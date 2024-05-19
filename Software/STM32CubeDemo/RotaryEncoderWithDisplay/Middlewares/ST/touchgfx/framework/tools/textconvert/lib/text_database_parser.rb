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
require 'nokogiri'
require 'lib/text_entries'
require 'lib/typographies'

class ParserBase
  def initialize(xml_doc)
    @xml_doc = xml_doc
  end

protected
  def empty_to_nil(str)
    str ? str.empty? ? nil : str : nil
  end

  def yes_to_true(str)
    empty_to_nil(str)
    !str.nil? && str.downcase == "yes" ? true : false
  end
end

class TextDatabaseParser < ParserBase
  def run
    languages = LanguageParser.new(@xml_doc).run
    typographies = TypographyParser.new(@xml_doc, languages).run
    text_entries = TextParser.new(@xml_doc, languages, typographies).run

    return languages, typographies.get_typographies(), text_entries
  end
end

class LanguageParser < ParserBase
  def run
    languages = []
    @xml_doc.xpath("/TextDatabase/Languages/Language").each do |lang_node|
      language = empty_to_nil(lang_node["Id"])
      fail "ERROR: Attribute 'Id' not specified in line #{lang_node.line} in #{lang_node.to_s}" if language.nil?
      if !language.match(/^([0-9a-zA-Z_])*$/)
        fail "ERROR: Illegal characters found in line #{lang_node.line} for <Language Id=\"#{language}\">"
      end
      languages.push(language)
    end

    return languages
  end
end

class TypographyParser < ParserBase
  def initialize(xml_doc, languages)
    super(xml_doc)
    @languages = languages
    @typographies = Typographies.new
  end

  def run
    @xml_doc.xpath("/TextDatabase/Typographies/Typography").each do |typo_node|
      typo_id = empty_to_nil(typo_node["Id"])
      font = empty_to_nil(typo_node["Font"])
      size = empty_to_nil(typo_node["Size"])
      bpp = empty_to_nil(typo_node["Bpp"])
      direction = empty_to_nil(typo_node["Direction"])
      is_vector = yes_to_true(typo_node["IsVector"])
      fallback_character = empty_to_nil(typo_node["FallbackCharacter"])
      wildcard_characters = empty_to_nil(typo_node["WildcardCharacters"])
      wildcard_widget_characters = empty_to_nil(typo_node["WidgetWildcardCharacters"])
      wildcard_character_ranges = empty_to_nil(typo_node["WildcardCharacterRanges"])
      ellipsis_character = empty_to_nil(typo_node["EllipsisCharacter"])

      fail "ERROR: Attribute 'Id' not specified in line #{typo_node.line} in #{typo_node.to_s}" if typo_id.nil?
      fail "ERROR: Attribute 'Font' not specified in line #{typo_node.line} for <Typography Id=\"#{typo_id}\">" if font.nil?
      fail "ERROR: Attribute 'Size' not specified in line #{typo_node.line} for <Typography Id=\"#{typo_id}\">" if size.nil?
      fail "ERROR: Attribute 'Bpp' not specified in line #{typo_node.line} for <Typography Id=\"#{typo_id}\">" if bpp.nil?
      fail "ERROR: Attribute 'Direction' not specified in line #{typo_node.line} for <Typography Id=\"#{typo_id}\">" if direction.nil?
      fail "ERROR: Attribute 'IsVector' not specified in line #{typo_node.line} for <Typography Id=\"#{typo_id}\">" if is_vector.nil?
      if !typo_id.match(/^([0-9a-zA-Z_])*$/)
        fail "ERROR: Illegal characters found in line #{typo_node.line} for <Typography Id=\"#{typo_id}\">"
      end

      # Default typography
      @typographies.add(typo_id, "", font, size, bpp, is_vector, fallback_character, ellipsis_character, wildcard_characters, wildcard_widget_characters, wildcard_character_ranges, direction)

      typo_node.xpath('./LanguageSetting').each do |language_setting|
        language = empty_to_nil(language_setting["Language"])
        font = empty_to_nil(language_setting["Font"])
        size = empty_to_nil(language_setting["Size"])
        bpp = empty_to_nil(language_setting["Bpp"])
        direction = empty_to_nil(language_setting["Direction"])
        is_vector = yes_to_true(language_setting["IsVector"])
        fallback_character = empty_to_nil(language_setting["FallbackCharacter"])
        wildcard_characters = empty_to_nil(language_setting["WildcardCharacters"])
        wildcard_widget_characters = empty_to_nil(language_setting["WidgetWildcardCharacters"])
        wildcard_character_ranges = empty_to_nil(language_setting["WildcardCharacterRanges"])
        ellipsis_character = empty_to_nil(language_setting["EllipsisCharacter"])

        fail "ERROR: Attribute 'Language' not specified in line #{language_setting.line} in #{language_setting.to_s}" if language.nil?
        fail "ERROR: Attribute 'Font' not specified in line #{language_setting.line} for <LanguageSetting Language=\"#{language}\">" if font.nil?
        fail "ERROR: Attribute 'Size' not specified in line #{language_setting.line} for <LanguageSetting Language=\"#{language}\">" if size.nil?
        fail "ERROR: Attribute 'Bpp' not specified in line #{language_setting.line} for <LanguageSetting Language=\"#{language}\">" if bpp.nil?
        fail "ERROR: Attribute 'Direction' not specified in line #{language_setting.line} for <LanguageSetting Language=\"#{language}\">" if direction.nil?
        if !language.match(/^([0-9a-zA-Z_])*$/)
          fail "ERROR: Illegal characters found in line #{language_setting.line} for <LanguageSetting Language=\"#{language}\">"
        end
        fail "ERROR: Unknown language '#{language}'" if !@languages.include?(language)

        # Language specific typography
        @typographies.add(typo_id, language, font, size, bpp, is_vector, fallback_character, ellipsis_character, wildcard_characters, wildcard_widget_characters, wildcard_character_ranges, direction)
      end
    end
    return @typographies
  end
end

class TextParser < ParserBase
  def initialize(xml_doc, languages, typographies)
    super(xml_doc)
    @languages = languages
    @typographies = typographies
    @text_entries = TextEntries.new
  end

  def run
    @xml_doc.xpath("/TextDatabase/Texts/TextGroup/Text").each do |text_node|
      text_id = empty_to_nil(text_node["Id"])
      default_typography_id = empty_to_nil(text_node["TypographyId"])
      default_alignment = empty_to_nil(text_node["Alignment"])

      fail "ERROR: Attribute 'Id' not specified in line #{text_node.line} in #{text_node.to_s}" if text_id.nil?
      fail "ERROR: Attribute 'TypographyId' not specified in line #{text_node.line} for <Text Id=\"#{text_id}\">" if default_typography_id.nil?
      fail "ERROR: Attribute 'Alignment' not specified in line #{text_node.line} for <Text Id=\"#{text_id}\">" if default_alignment.nil?
      if !text_id.match(/^([0-9a-zA-Z_])*$/)
        fail "ERROR: Illegal characters found in line #{text_node.line} for <Text Id=\"#{text_id}\">"
      end

      default_typography_name = @typographies.get_typography_name(default_typography_id, "")
      default_direction = @typographies.get_direction(default_typography_id, "")

      text_entry = TextEntry.new(text_id, default_typography_name, default_alignment, default_direction);

      fail "ERROR: Translation not specified in line #{text_node.line} for <Text Id=\"#{text_id}\">" if !text_node.at("Translation")

      text_node.xpath('./Translation').each do |translation|
        language = empty_to_nil(translation["Language"])
        specific_alignment = empty_to_nil(translation["Alignment"])

        fail "ERROR: Attribute 'Language' not specified in line #{translation.line} for Translation" if language.nil?
        if !language.match(/^([0-9a-zA-Z_])*$/)
          fail "ERROR: Illegal characters found in line #{translation.line} for <Translation Language=\"#{language}\">"
        end
        fail "ERROR: Unknown language '#{language}'" if !@languages.include?(language)

        if translation.text.match(/\n\t|\n /)
          puts "WARNING: Text in line #{translation.line} for <Text Id=\"#{text_id}\"> with Language '#{language}' contains tabs or whitespace indentation"
        end

        specific_typography_name = @typographies.get_typography_name(default_typography_id, language)
        specific_direction = @typographies.get_direction(default_typography_id, language)

        specific_typography_name = nil if specific_typography_name == default_typography_name
        specific_direction = nil if specific_direction == default_direction
        specific_alignment = nil if specific_alignment == default_alignment

        text_entry.add_translation(language, translation.text)
        text_entry.add_typography(language, specific_typography_name) if !specific_typography_name.nil?
        text_entry.add_direction(language, specific_direction) if !specific_direction.nil?
        text_entry.add_alignment(language, specific_alignment) if !specific_alignment.nil?
      end

      @text_entries.add(text_entry)
    end
    return @text_entries
  end
end
