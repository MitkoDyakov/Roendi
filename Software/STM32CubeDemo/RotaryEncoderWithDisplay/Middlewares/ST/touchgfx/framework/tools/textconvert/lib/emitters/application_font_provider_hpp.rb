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
class ApplicationFontProviderHpp < Template
  def initialize(text_entries, typographies, languages, output_directory, generate_font_format)
    super(text_entries, typographies, languages, output_directory)
    @generate_font_format = generate_font_format
    @cache = {}
    #map typographies to index of first using same font = font index
    @font_index = @typographies.inject(Hash.new) do |list, typography|
      name = typography_name(typography)
      list[name] ||= list.size
      list
    end
  end
  def input_path
    File.join(root_dir,'Templates','ApplicationFontProvider.hpp.temp')
  end
  def output_path
    '/include/fonts/ApplicationFontProvider.hpp'
  end
  def cache_file
    File.join(@output_directory, 'cache/ApplicationFontProviderHpp.cache')
  end
  def output_filename
    File.join(@output_directory, output_path)
  end
  def run
    @cache["typographies"] = @typographies.collect{|t| [t.name, t.font_file, t.font_size, t.bpp] }
    @cache["generate_font_format"] = @generate_font_format
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
      #generate ApplicationFontProvider.hpp
      super
    end
  end

  def save_flashreader?
    @generate_font_format == "1"
  end

  def get_font_index(index)
    @font_index[typography_name(@typographies[index])]
  end

  def get_max_font_index
    @font_index.size
  end

  def get_font_comment_space(index)
    @max_length ||= @typographies.map { |t| t.name.length }.max
    ' ' * (@max_length - @typographies[index].name.length)
  end

  def get_font_comment(index)
    typography_name(@typographies[index])
  end

  private

  def typography_name(typography)
    if typography.is_vector
      "Vector_#{typography.cpp_name}_#{typography.font_size}"
    else
      "#{typography.cpp_name}_#{typography.font_size}_#{typography.bpp}bpp"
    end
  end
end
