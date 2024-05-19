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
# coding: iso-8859-1
require 'nokogiri'
require 'rubyxl'
require 'rubyXL/convenience_methods'
require 'lib/text_entries'
require 'lib/xml_reader'
require 'lib/xml_writer'

# From https://github.com/weshatheleopard/rubyXL/wiki/How-to
class RubyXL::Cell
  def unlock
    xf = get_cell_xf.dup
    xf.protection = xf.protection&.dup || RubyXL::Protection.new
    xf.protection.locked = false
    xf.apply_protection = true
    self.style_index = workbook.register_new_xf(xf)
  end
end
class RubyXL::Worksheet
  def add_frozen_split(row:, column:)
    worksheetview = RubyXL::WorksheetView.new
    worksheetview.pane = RubyXL::Pane.new(:top_left_cell => RubyXL::Reference.new(row,column),
                                          :y_split => row,
                                          :x_split => column,
                                          :state => 'frozenSplit',
                                          :activePane => 'bottomRight')
    worksheetviews = RubyXL::WorksheetViews.new
    worksheetviews << worksheetview
    self.sheet_views = worksheetviews
  end
end

class TranslationIO
  def initialize(file_name, translation_name)
    @xml_doc = XMLReader.new.read(file_name)
    @translation_name = translation_name
    @file_name = file_name
    @xml_file_version = @xml_doc.at("TextDatabase")["Version"]
  end

  WHITE = 'FFFFFF'
  BLACK = '000000'

  HEADER_BACKGROUND = '4472C4'
  HEADER_FOREGROUND = WHITE
  FIRST_ROW_BACKGROUND = 'B4C6E7'
  FIRST_ROW_FOREGROUND = BLACK
  SECOND_ROW_BACKGROUND = 'D9E1F2'
  SECOND_ROW_FOREGROUND = BLACK

  SHEET_NAME = 'TouchGFX Translation'
  TEXT_ID = 'Text Id'

  def exportExcel(languages)
    workbook = RubyXL::Workbook.new
    worksheet = workbook[0]
    worksheet.sheet_name = SHEET_NAME

    existing_languages = get_languages
    if languages.empty?
      # Empty string means "put TextID in this column"
      languages = [ '' ] + existing_languages
    else
      # Empty string means "put TextID in this column"
      languages = [''] + languages.map do |lang|
        lang_upcase = lang.upcase
        actual_language = existing_languages.find { |l| l.upcase == lang_upcase }
        fail "ERROR: Unknown export language: #{lang}" if !actual_language
        actual_language
      end
    end
    languages.uniq!
    languages.each_with_index do |lang, column|
      cell = worksheet.add_cell(0, column, lang.empty? ? TEXT_ID : lang)
      cell.change_font_color(HEADER_FOREGROUND)
      cell.change_fill(HEADER_BACKGROUND)
      cell.change_font_bold(true)
      worksheet.change_column_width(column, lang.empty? ? 20 : 30)
    end

    # This line is only needed if the font_size of each cell is to be updated (inside the loop below)
    # typography_map = @typographies.inject({}) { |map,typo| map[typo.name] = typo; map }
    get_text_entries.each_with_index do |text, row|
      languages.each_with_index do |lang, column|
        cell = worksheet.add_cell(row+1, column, lang.empty? ? text.text_id : text.translation_in(lang))
        cell.change_vertical_alignment('top')
        if (row % 2) == 0
          cell.change_font_color(FIRST_ROW_FOREGROUND)
          cell.change_fill(FIRST_ROW_BACKGROUND)
        else
          cell.change_font_color(SECOND_ROW_FOREGROUND)
          cell.change_fill(SECOND_ROW_BACKGROUND)
        end
        cell.change_font_bold(lang.empty?)
        cell.change_border(:bottom, 'thin')
        if !lang.empty?
          cell.change_border(:right, 'thin')
          cell.change_text_wrap(true)
          # Lines only needed if the font size of each cell is to be updated
          #typography_name = text.typographies[lang] || text.typography
          #cell.change_font_size((typography_map[typography_name].font_size / 1.5).to_i)
          alignment = text.alignment_in(lang) || text.alignment
          cell.change_horizontal_alignment(alignment.downcase)
          cell.unlock
        end
      end
    end

    worksheet.add_frozen_split(:row => 1, :column => 1)
    worksheet.sheet_protection = RubyXL::WorksheetProtection.new(sheet: true, format_columns: false, format_rows: false)

    workbook.write(@translation_name)
  end

  def importExcel(languages)
    require 'lib/xml_writer'

    workbook = RubyXL::Parser.parse(@translation_name)
    worksheet = workbook.worksheets.find { |sheet| sheet.sheet_name == SHEET_NAME }
    fail "ERROR: \"#{@translation_name}\" does not contain a sheet called \"#{SHEET_NAME}\"" if !worksheet

    existing_languages = get_languages

    header = [] # Collect the header with correctly capitalized languages
    text_id_column = nil # Which column contains the TEXT_ID
    import_columns = [] # Which columns to import
    column = 0
    while column < worksheet[0].size
      if worksheet[0][column]
        lang_cell = worksheet[0][column].value
        if !lang_cell.empty?
          lang_upcase = lang_cell.upcase
          if lang_upcase == TEXT_ID.upcase
            text_id_column = column
            fail "ERROR: Multiple columns contain \"#{TEXT_ID}\"" if header.include?('')
            header << ''
          else
            # Find the language with the correct capitalization
            orig_lang = existing_languages.find { |l| l.upcase == lang_upcase }
            # Fail if all languages should be imported AND the language from the spreadsheet is illegal
            fail "ERROR: Text Database does not contain language \"#{lang_cell}\", create the language in the TouchGFX Designer" if languages.empty? && !orig_lang && text_id_column
            # if no languages specified, import all. Otherwise only import if language is wanted
            if languages.empty? || languages.any? { |l| l.upcase == lang_upcase }
              import_columns += [ column ]
            end
            fail "ERROR: Multiple columns contain translations for language \"#{orig_lang}\"" if header.include?(orig_lang)
            header << orig_lang if not orig_lang.nil?
          end
        end
      end
      column += 1
    end

    upper_languages = languages.map(&:upcase)
    upper_existing_languages = existing_languages.map(&:upcase)
    # Did we ask to import a language (on the command line) which does not exist in the spreadsheet?
    fail "ERROR: Unknown language(s) #{(upper_languages - upper_existing_languages)*','}" if !(upper_languages - upper_existing_languages).empty?
    fail "ERROR: Missing column \"#{TEXT_ID}\"" if !text_id_column

    text_nodes = get_text_nodes_map
    # Row 0 is the header
    row = 1
    all_text_ids = []
    while row < worksheet.sheet_data.rows.size
      if worksheet[row] && worksheet[row][text_id_column]
        text_id = worksheet[row][text_id_column].value
        if text_id && !text_id.empty?
          fail "ERROR: Extra translations of Text Id \"#{text_id}\" given in line #{row}" if all_text_ids.include?(text_id)
          import_columns.each do |column|
            text_node = text_nodes[text_id]
            fail "ERROR: The Text Id \"#{text_id}\" in line #{row} does not exist in the database" if !text_node
            cell = worksheet[row][column]
            cell_text = cell ? cell.value.to_s : ''
            set_text_node_translation(text_node, header[column], cell_text)
            #puts "Setting #{text_id}.#{header[column]} = #{worksheet[row][column].value}"
          end
          all_text_ids << text_id
        end
      end
      row += 1
    end

    all_predefined_ids = get_text_ids
    if !(all_predefined_ids - all_text_ids).empty?
      puts "WARNING: \"#{@translation_name}\" does not contain the following Text Id's: #{(all_predefined_ids - all_text_ids)*', '}"
    end

    XMLWriter.new.write(@file_name, @xml_doc)
  end

protected
  def empty_to_nil(str)
    str ? str.strip.empty? ? nil : str.strip : nil
  end

  #Get array of all Languages in XML
  def get_languages
    @xml_doc.xpath("/TextDatabase/Languages/Language").inject([]) do |languages, lang_node|
      language = empty_to_nil(lang_node["Id"])
      languages.push(language)
    end
  end

  #Get array of all text IDs in XML
  def get_text_ids
    @xml_doc.xpath("/TextDatabase/Texts/TextGroup/Text").map{ |text_node| text_node["Id"] }
  end

  #Map of textId to XML node
  def get_text_nodes_map
    @xml_doc.xpath("/TextDatabase/Texts/TextGroup/Text").inject({}) do |nodes, text_node|
      nodes[text_node["Id"]] = text_node
      nodes
    end
  end

  #Update the translation of a text
  def set_text_node_translation(text_node, language, new_translation)
    text_node.xpath("./Translation").each do |translation_node|
      if translation_node["Language"] == language
        #Remove old translation
        translation_node.child.remove if translation_node.child

        translation_node.add_child(Nokogiri::XML::Text.new(new_translation, @xml_doc))
      end
    end
  end

  #Class holding single text with ID, default alignment, translations, and alignments
  class TextEntry
    def initialize(text_id, alignment)
      @text_id = text_id
      @alignment = alignment
      @alignments = {}    # Language -> alignment
      @translations = {}  # Language -> text (translation)
    end
    def text_id
      @text_id
    end
    def translation_in(language)
      @translations[language]
    end
    def alignment
      @alignment
    end
    def alignment_in(language)
      @alignments[language] || alignment
    end
    def set_alignment(language, alignment)
      @alignments[language] = alignment
    end
    def set_translation(language, text)
      @translations[language] = text
    end
  end

  #Compute array of TextEntry objects
  def get_text_entries
    texts = []
    @xml_doc.xpath("/TextDatabase/Texts/TextGroup/Text").each do |text_node|
      text = TextEntry.new(text_node["Id"], text_node["Alignment"])
      text_node.search("Translation").each do |translation|
        language = translation["Language"]
        alignment = translation["Alignment"]
        text.set_alignment(language, alignment) unless alignment.nil?
        text.set_translation(language, translation.text)
      end
      texts << text
    end
    texts
  end
end
