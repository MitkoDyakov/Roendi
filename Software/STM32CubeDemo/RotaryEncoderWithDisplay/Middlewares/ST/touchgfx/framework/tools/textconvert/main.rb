# Copyright (c) 2018(-2024) STMicroelectronics.
# All rights reserved.
#
# This file is part of the TouchGFX 4.23.2 distribution.
#
# This software is licensed under terms that can be found in the LICENSE file in
# the root directory of this software component.
# If no LICENSE file comes with this software, it is provided AS-IS.
#
###############################################################################/
$:.unshift File.dirname(__FILE__)

require 'lib/version'

WINDOWS_LINE_ENDINGS = "\r\n"
UNIX_LINE_ENDINGS = "\n"
#on windows/mingw file.write will it self translate \n to \r\n, on linux not
LINE_ENDINGS = RUBY_PLATFORM.match(/linux/) ? WINDOWS_LINE_ENDINGS : UNIX_LINE_ENDINGS

def root_dir
  # Get the dirname of this (main.rb) file:
  @root_dir ||= File.dirname(__FILE__)
end

class Main

  def self.banner
    <<-BANNER
Create binary and cpp text files from Text Database

Usage: #{File.basename($0)} file.xml path/to/fontconvert.out path/to/fonts_output_dir path/to/localization_output_dir path/to/font/asset calling_path {remap|yes|no} {A1|A2|A4|A8} {binary_translations} {binary_fonts} {RGB565|RGB888|BW|GRAY2|GRAY4|ARGB2222|ABGR2222|RGBA2222|BGRA2222}

Where 'remap'/'yes' will map identical texts to the same memory area to save space
      'A1'/'A2'/'A4'/'A8' will generate fonts in the given format
      'binary_translations' will generate binary translations instead of cpp files
      'binary_fonts' will generate binary font files instead of cpp files
      last argument is the framebuffer format (used to limit the bit depth of the generated fonts)
      Configuration specified in the application.config file take precedence over the command line arguments
BANNER
  end

  def self.upgrade
    <<-UPGRADE

---------------------------------------------------------------------------
Your TouchGFX Environment is using an old Ruby version (#{RUBY_VERSION}).
TouchGFX #{TOUCHGFX_VERSION} uses Ruby version 3.
Please use the new TouchGFX Environment.
---------------------------------------------------------------------------

UPGRADE
  end

  def self.missing_files
    return !File.exists?("#{@fonts_output_path}/include/fonts/ApplicationFontProvider.hpp") ||
           !File.exists?("#{@localization_output_path}/include/texts/TextKeysAndLanguages.hpp")
  end

  if Integer(RUBY_VERSION.match(/\d+/)[0]) < 3 && !RUBY_PLATFORM.match(/linux/)
    puts self.upgrade
  end

  if __FILE__ == $0

    if ARGV.count < 6
      abort self.banner
    end

    file_name = ARGV.shift
    font_convert_path = ARGV.shift
    @fonts_output_path = ARGV.shift
    @localization_output_path = ARGV.shift
    font_asset_path = ARGV.shift
    $calling_path = ARGV.shift

    #optional arguments
    remap_global = ARGV.include?("yes") || ARGV.include?("remap") ? "yes" : "no"
    autohint_setting = "default"

    data_format_a1 = ARGV.include?("A1") ? "A1" : ""
    data_format_a2 = ARGV.include?("A2") ? "A2" : ""
    data_format_a4 = ARGV.include?("A4") ? "A4" : ""
    data_format_a8 = ARGV.include?("A8") ? "A8" : ""

    generate_binary_translations = ARGV.include?("binary_translations") ? "yes" : "no"
    generate_binary_fonts = ARGV.include?("binary_fonts") ? "yes" : "no"

    framebuffer_bpp = ""
    ["BPP32", "BPP24", "BPP16", "BPP8", "BPP4", "BPP2", "BPP1"].each do |format|
      if ARGV.include?(format)
        framebuffer_bpp = format
      end
    end

    generate_font_format = "0" # 0 = normal font format, 1 = unmapped_flash_font_format

    korean_fusion_fonts = []

    require 'json'

    application_config = File.join($calling_path, "application.config")
    if File.file?(application_config)
      text_conf = JSON.parse(File.read(application_config))["text_configuration"] || {}

      remap = text_conf["remap"]
      if remap
        remap_global = remap == "yes" ? "yes" : "no"
      end

      autohint = text_conf["autohint"]
      if autohint
        autohint_setting = (autohint == "no" || autohint == "force") ? autohint : "default"
      end

      a1 = text_conf["a1"]
      if a1
        data_format_a1 = a1 == "yes" ? "A1" : ""
      end
      a2 = text_conf["a2"]
      if a2
        data_format_a2 = a2 == "yes" ? "A2" : ""
      end
      a4 = text_conf["a4"]
      if a4
        data_format_a4 = a4 == "yes" ? "A4" : ""
      end
      a8 = text_conf["a8"]
      if a8
        data_format_a8 = a8 == "yes" ? "A8" : ""
      end

      binary_translations = text_conf["binary_translations"]
      if binary_translations
        generate_binary_translations = binary_translations == "yes" ? "yes" : "no"
      end

      binary_fonts = text_conf["binary_fonts"]
      if binary_fonts
        generate_binary_fonts = binary_fonts == "yes" ? "yes" : "no"
      end

      bpp = text_conf["framebuffer_bpp"]
      if bpp
        framebuffer_bpp = "BPP" + bpp
      end

      font_format = text_conf["font_format"]
      if font_format
        values = ["0", "1"]
        if values.include? font_format
          generate_font_format = font_format
        else
          puts "Font format #{font_format} not correct, using default: \"0\""
        end
      end

      fusion_fonts = text_conf["korean_fusion_fonts"]
      if fusion_fonts
        korean_fusion_fonts = fusion_fonts
      end
    end

    remap_global ||= "no"

    data_format = "#{data_format_a1}#{data_format_a2}#{data_format_a4}#{data_format_a8}"
    if generate_binary_translations == "yes" && remap_global == "yes"
      puts "Disabling global remapping of identical texts, because binary language files are generated"
      remap_global = "no"
    end

    begin
      # 0. check text database file extension. Allow texts.xlsx as parameter, but require a texts.xml to be present
      # 1. if text_converter/font_converter is newer than compile_time.cache, remove all files under generated/texts and generated/fonts
      # 1b if generated/fonts/include/fonts/ApplicationFontProvider.hpp is missing, force generation of TextKeysAndLanguages.hpp
      # 1c if generated/texts/cache/options.cache contents differ from supplies arguments, force run
      # 2. if generated/texts/cache/compile_time.cache is newer than xml file and fonts/ApplicationFontProvider.hpp exists then stop now
      # 3. remove UnicodeList*.txt and CharSizes*.csv
      # 4. create #{@localization_output_path}/include/texts/ and #{@fonts_output_path}/include/fonts/

      require 'fileutils'

      # 0:
      if file_name.match(/\.xlsx$/)
        xml_file_name = file_name.gsub(/\.xlsx$/, '.xml')
        if File.exists?(xml_file_name)
          if File.exists?(file_name)
            puts "WARNING: Using \"#{xml_file_name}\" instead of \"#{file_name}\""
          end
        else
          fail "ERROR: #{xml_file_name} not found"
        end
        file_name = xml_file_name
      end

      # 1:
      text_converter_time = Dir[File.join(__dir__,'**','*'), font_convert_path].collect{|f| [File.mtime(f), File.ctime(f)]}.flatten.max

      if ((compile_time_exists = File.exists?("#{@localization_output_path}/cache/compile_time.cache")) && text_converter_time > File.mtime("#{@localization_output_path}/cache/compile_time.cache")) || !compile_time_exists
        #remove all files, as text converter is newer (probably upgraded to new TouchGFX)
        puts "Cleaning generated files from #{@localization_output_path} and #{@fonts_output_path}."
        if @localization_output_path.match /generated\/texts$/
          local_path = @localization_output_path.gsub('\\','/')
          FileUtils.rm_rf("#{local_path}")
        end
        if @fonts_output_path.match /generated\/fonts$/
          local_path = @fonts_output_path.gsub('\\','/')
          FileUtils.rm_rf("#{local_path}")
        end
      end

      # 1b:
      $Force_Generate_TextKeysAndLanguages = self.missing_files

      # 1c:
      force_run = false
      options_file = "#{@localization_output_path}/cache/options.cache"
      options = File.exists?(options_file) && File.read(options_file)

      new_options = { :remap => remap_global,
                      :autohint => autohint_setting,
                      :data_format => data_format,
                      :binary_translations => generate_binary_translations,
                      :binary_fonts => generate_binary_fonts,
                      :font_format => generate_font_format,
                      :framebuffer_bpp => framebuffer_bpp,
                      :fusion_fonts => fusion_fonts }.to_json

      if (options != new_options)
        force_run = true
        require 'lib/file_io'
        FileIO.write_file_silent(options_file, new_options)
      end

      # 2:
      if File.exists?("#{@localization_output_path}/cache/compile_time.cache") && !self.missing_files && !force_run
        mod_time = [File.mtime(file_name), File.ctime(file_name)].max
        if mod_time < File.mtime("#{@localization_output_path}/cache/compile_time.cache")
          exit
        end
      end

      # 3:
      Dir["#{@fonts_output_path}/UnicodeList*.txt"].each do |text_file|
        FileUtils.rm_f(text_file)
      end
      Dir["#{@fonts_output_path}/VectorUnicodeList*.txt"].each do |text_file|
        FileUtils.rm_f(text_file)
      end
      Dir["#{@fonts_output_path}/CharSizes*.csv"].each do |text_file|
        FileUtils.rm_f(text_file)
      end

      # 4:
      FileUtils.mkdir_p("#{@localization_output_path}/include/texts/")
      FileUtils.mkdir_p("#{@fonts_output_path}/include/fonts")

      require 'lib/emitters/fonts_cpp'
      require 'lib/generator'
      FontsCpp.font_convert = font_convert_path
      Generator.new.run(file_name, @fonts_output_path, @localization_output_path, font_asset_path, data_format, remap_global, autohint_setting, generate_binary_translations, generate_binary_fonts, framebuffer_bpp, generate_font_format, korean_fusion_fonts)
      #touch the cache compile time that we rely on in the makefile
      FileUtils.touch "#{@localization_output_path}/cache/compile_time.cache"

    rescue SystemExit => e
      if e.status != 0
        abort "" #Do not print anything, already done in original abort call
      end

    rescue Exception => e
      STDERR.puts e
      STDERR.puts e.backtrace if ENV['DEBUG']
      abort "An error occurred during text conversion"
    end
  end
end
