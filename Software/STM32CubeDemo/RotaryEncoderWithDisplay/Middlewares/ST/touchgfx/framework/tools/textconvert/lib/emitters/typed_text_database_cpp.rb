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
class TypedTextDatabaseCpp < Template
  TypedTextPresenter = Struct.new(:alignment, :direction, :typography)

  def initialize(text_entries, typographies, languages, output_directory, generate_binary_translations, generate_font_format)
    super(text_entries, typographies, languages, output_directory)
    @generate_binary_translations = generate_binary_translations
    @generate_font_format = generate_font_format
    @cache = {}
  end

  def cache_file
    File.join(@output_directory, 'cache/TypedTextDatabaseCpp.cache')
  end
  def input_path
    File.join(root_dir,'Templates','TypedTextDatabase.cpp.temp')
  end
  def output_path
    'src/TypedTextDatabase.cpp'
  end
  def output_filename
    File.join(@output_directory, output_path)
  end

  def run
    #compute the typed text table once
    compute_typed_texts

    #calculate the cache map
    #first the layout databases
    databases = {}
    get_layouts.each do |layout|
      typed_texts = get_typed_texts(layout)
      typed_text_db = []
      typed_texts.inject(typed_text_db) do |a, typed_text|
        #each text has a font index, alignment and direction
        fontIdx = get_font_index(typed_text.typography)
        a << [fontIdx, typed_text.alignment.upcase, typed_text.direction.upcase]
      end
      databases[layout] = typed_text_db
    end
    #now the list of typed text databases
    language_db_list = []
    @languages.inject(language_db_list) do |list, lang|
      list << (get_layouts.find{|l|l==lang}||'DEFAULT');list
    end
    @cache["databases"] = databases
    @cache["database_list"]=language_db_list
    @cache["fonts"] = fontmap
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
      #generate TypedTextDatabase.cpp
      super
    end
  end

  def has_text_entries?
    !text_entries.empty?
  end

  def get_font_class_names
    includes = []
    if @generate_font_format == "1"
      includes << "UnmappedDataFont"
      #see if we have any vector fonts, then we also need the GeneratedVectorFont.hpp include
      if typographies.any?{|t| t.is_vector }
        includes << "GeneratedFont"
      end
    else
      includes << "GeneratedFont"
    end
    includes
  end

  def get_font_class_name(font)
    if font=~/^getFont_vector_/
      "GeneratedVectorFont"
    else
      @generate_font_format == "1" ? "UnmappedDataFont" : "GeneratedFont"
    end
  end

  def get_touchgfx_aligment(alignment)
    "touchgfx::#{alignment.upcase}"
  end

  def get_touchgfx_direction(direction)
    "touchgfx::TEXT_DIRECTION_#{direction.upcase}"
  end

  def get_font_index(typography)
    fontmap[get_getFont_name(typography)]
  end

  def get_fonts
    @cached_fonts ||=
      begin
        typographies.map{ |t| Typography.new("", t.font_file, t.font_size, t.bpp, t.is_vector) }.uniq.collect do |t|
          get_getFont_name(t)
        end
      end
  end

  def get_typed_texts(layout)
    @typed_texts[layout]
  end

  def get_layouts
    @cached_layouts ||=
      begin
        if text_entries.empty?
          ["DEFAULT"]
        else
          (text_entries.languages_with_specific_settings << "DEFAULT").uniq
        end
      end
  end

  def get_languages
    @languages
  end

  def compute_typed_texts
    @typed_texts = {}
    get_layouts.each do |layout|
      @typed_texts[layout] = text_entries.collect do |entry|
        typography_name = entry.typographies[layout] || entry.default_typography
        typography = typographies.find { |t| t.name == typography_name }
        alignment = entry.alignments[layout] || entry.default_alignment
        direction = entry.directions[layout] || entry.default_direction
        TypedTextPresenter.new(alignment, direction, typography);
      end
    end
  end

  def generate_binary_files?
    @generate_binary_translations=="yes"
  end

  def fontmap
    @fontmap ||=
      begin
        @fontmap = Hash.new
        get_fonts.each_with_index do |f, i|
          fontmap[f] = i
        end
        fontmap
      end
  end
end
