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
require 'lib/excel_reader'
require 'lib/text_entries'
require 'lib/typographies'

class TextDatabaseParser_4_17
  def initialize(file_name)
    @file_name = file_name
  end

  def run
    typographies = TypographyParser_4_17.new(@file_name).run
    text_entries = TextParser_4_17.new(@file_name).run
    return typographies, text_entries
  end
end

class TypographyParser_4_17
  def initialize(file_name)
    header_row_number = 3
    header_column_number = 2
    @reader = ExcelReader.new(file_name, 'Typography', header_row_number, header_column_number)
  end

  def run
    typographies = []
    @reader.read_rows do |row|
      name = row[:'Typography Name']
      font = row[:font]
      size = row[:size]
      bpp = row[:bpp]
      if row.exists?(:'Fallback Character')
        fallback_character = row[:'Fallback Character']
      end
      if row.exists?(:'Wildcard Characters')
        wildcard_characters = row[:'Wildcard Characters']
      end
      if row.exists?(:'Widget Wildcard Characters')
        widget_wildcard_characters = row[:'Widget Wildcard Characters']
      end
      if row.exists?(:'Character Ranges') # New name
        wildcard_ranges = row[:'Character Ranges']
      elsif row.exists?(:'Wildcard Ranges') # Old name
        wildcard_ranges = row[:'Wildcard Ranges']
      end
      if row.exists?(:'Ellipsis Character')
        ellipsis_character = row[:'Ellipsis Character']
      end

      if name
        name.strip!
        unless name.match(/^([0-9a-zA-Z_])*$/)
          fail "ERROR: Illegal characters found in Text ID '#{name}'"
        end
      end
      font = font.strip if font
      size = size.strip if size
      bpp = bpp.strip if bpp
      fallback_character = fallback_character.strip if fallback_character
      wildcard_characters = wildcard_characters.strip if wildcard_characters
      widget_wildcard_characters = widget_wildcard_characters.strip if widget_wildcard_characters
      wildcard_ranges = wildcard_ranges.strip if wildcard_ranges
      ellipsis_character = ellipsis_character.strip if ellipsis_character

      if name && font && size && bpp
        typographies.push Typography.new(name, font, size.to_i, bpp.to_i, false, fallback_character, ellipsis_character, wildcard_characters, widget_wildcard_characters, wildcard_ranges)
      end
    end
    typographies
  end
end

class TextParser_4_17
  def initialize(file_name)
    header_row_number = 3
    header_column_number = 2
    @reader = ExcelReader.new(file_name, "Translation", header_row_number, header_column_number)
  end

  def language_capitalization(lang)
    lang_upcase = lang.upcase
    @languages.find { |l| l.upcase == lang_upcase }
  end

  def run
    @reader.read_header do |header|
      @alignments = header.select { |column| column.match(/^.*-ALIGNMENT$/i) }
      @directions = header.select { |column| column.match(/^.*-DIRECTION$/i) }
      @typographies = header.select { |column| column.match(/^.*-TYPOGRAPHY$/i) }
      @languages = header.select { |column| column.match(/^(\w{1,3})$/i ) }
    end

    # Check for undefined languages in language specific typographies
    @typographies.each do |typography|
      language, _ = typography.upcase.split('-')
      fail "ERROR: Unknown language in column #{language}-TYPOGRAPHY" if !@languages.any?{ |lang| lang.upcase == language }
    end

    # Check for undefined languages in language specific alignments
    @alignments.each do |alignment|
      language, _ = alignment.upcase.split('-')
      fail "ERROR: Unknown language in column #{language}-ALIGNMENT" if not @languages.any?{ |lang| lang.upcase == language }
    end

    # Check for undefined languages in language specific directions
    @directions.each do |direction|
      language, _ = direction.upcase.split('-')
      fail "ERROR: Unknown language in column #{language}-DIRECTION" if not @languages.any?{ |lang| lang.upcase == language }
    end

    text_entries = TextEntries.new
    @reader.read_rows do |row|
      text_id = row[:"Text ID"]
      default_typography = row[:"Typography Name"]
      default_alignment = row[:Alignment]
      if row.exists?(:Direction)
        default_direction = row[:Direction]
      end

      text_id = text_id.strip if text_id
      default_typography = default_typography.strip if default_typography
      default_alignment = default_alignment.strip if default_alignment
      default_direction = default_direction.strip if default_direction

      if text_id && default_typography
        unless text_id.match(/^([0-9a-zA-Z_])*$/)
          fail "ERROR: Illegal characters found in Text ID '#{text_id}'"
        end

        text_entry = TextEntry.new(text_id, default_typography, default_alignment, default_direction)

        @typographies.each do |typography|
          language, _ = typography.split('-')
          language = language_capitalization(language)
          t = row[typography]
          t = t.strip if t
          text_entry.add_typography(language, t)
        end

        @alignments.each do |alignment|
          language, _ = alignment.split('-')
          language = language_capitalization(language)
          a = row[alignment]
          a = a.strip if a
          text_entry.add_alignment(language, a)
        end

        @directions.each do |direction|
          language, _ = direction.split('-')
          language = language_capitalization(language)
          d = row[direction]
          d = d.strip if d
          text_entry.add_direction(language, d)
        end

        @languages.each do |language|
          # Do *not* strip leading/trailing whitespace from translations.
          text_entry.add_translation(language, row[language])
        end

        text_entries.add(text_entry)
      end
    end
    text_entries
  end
end
