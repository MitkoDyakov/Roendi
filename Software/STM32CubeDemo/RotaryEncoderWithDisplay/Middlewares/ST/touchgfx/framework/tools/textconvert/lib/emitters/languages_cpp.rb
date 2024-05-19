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
require 'json'
require 'lib/string_collector'

class LanguagesCpp
  def initialize(string_indices, characters, text_entries, languages, output_directory, remap_global, generate_binary_translations)
    @string_indices = string_indices #dictionary of all string indices into the characters array
    @characters = characters
    @text_entries = text_entries
    @languages = languages
    @output_directory = output_directory
    @remap_global = remap_global
    @generate_binary_translations = generate_binary_translations
  end
  def run
    # First remove any unused LanguageXX.cpp files (ie. remove
    # LanguageGB.cpp before creating LanguageGb.cpp on windows which
    # ignores case on filenames)
    Dir.glob("#{@output_directory}/src/Language*.cpp").each do |file|
      m = /Language(.*).cpp/.match(file)
      xx = m[1]
      if !@languages.any? { |l| l.capitalize == xx }
        File.delete(file) if File.exist?(file)
      end
    end

    @languages.each_with_index do |language, language_index|
      language_index = 0 if @remap_global=="yes"
      LanguageXxCpp.new(@string_indices, language_index, @characters, @text_entries, @output_directory, @remap_global, @generate_binary_translations, language).run
    end
  end
end

class LanguageXxCpp < Template
  Presenter = Struct.new(:text_id, :int_array)

  def initialize(string_indices, language_index, characters, text_entries, output_directory, remap_global, generate_binary_translations, language)
    @string_indices = string_indices #dictionary of all string indices into the characters array
    @characters = characters
    @remap_global = remap_global
    @generate_binary_translations = generate_binary_translations
    @language = language
    @language_index = language_index
    super(text_entries, [], [], output_directory)
    @cache = {}
  end

  def cache_file
    File.join(@output_directory, "cache/LanguageCpp_#{@language.capitalize}.cache")
  end
  def input_path
    File.join(root_dir, 'Templates', generate_binary_files? ? 'LanguageXX.cpp.bin.temp' : 'LanguageXX.cpp.temp')
  end
  def output_path
    "src/Language#{get_language}.cpp"
  end
  def output_filename
    File.join(@output_directory, output_path)
  end
  def texts
    @text_entries.entries.map(&:cpp_text_id)
  end
  def run
    #build cache dictionary
    @cache["remap"] = @remap_global
    @cache["language"] = @language
    @cache["language_index"] = @language_index
    #save text ids and index
    @cache["indices"] = get_text_entries.collect do |entry|
      [get_string_index(entry), entry.text_id]
    end #list of index,textid
    if @remap_global!="yes"
      @cache["characters"] = @characters[@language_index]
    end

    new_cache_file = false
    if not File::exists?(cache_file)
      new_cache_file = true
    else
      #cache file exists, compare data with cache file
      old_cache = JSON.parse(File.read(cache_file))
      new_cache_file = (old_cache != @cache)
    end

    if new_cache_file
      #write new cache file
      FileIO.write_file_silent(cache_file, @cache.to_json)
    end

    if !File::exists?(output_filename) || new_cache_file
      #generate LanguageXX.cpp
      super
    end
  end

  private

  def remap_global?
    @remap_global=="yes"
  end

  def get_language
    @language.capitalize
  end

  def get_text_entries
    #only generate entries once
    @cached_entries ||=
      begin
        present(@text_entries)
      end
  end

  def get_string_index_spaces(entry)
    last = get_text_entries.length-1
    @max_length ||= get_text_entries.each_with_index.map { |e,i| get_string_index(e).length + (i < last || last == 0 ? 1 : 0) }.max
    ' ' * (@max_length - get_string_index(entry).to_s.length)
  end

  def get_string_index(entry)
    index = @string_indices[@language_index][entry.int_array]
    index ||= 0
    index.to_s
  end

  def generate_binary_files?
    @generate_binary_translations=="yes"
  end

  def present(entries)
    entries.map do |entry|
      Presenter.new(entry.cpp_text_id, entry.translation_in(@language).unicodes)
    end
  end

  def get_substrings_and_offsets(lang_index)
    unicode_array_to_hex_offset_comment(@characters[lang_index])
  end
end
