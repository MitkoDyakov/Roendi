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
require 'rubygems/version'
require 'lib/text_database_parser_4_17'
require 'lib/sanitizer'
require 'lib/version'
require 'lib/xml_reader'
require 'lib/xml_writer'

class TextDatabaseUpgrader
  def initialize(file_name, upgrade_version)
    @file_name = file_name
    @upgrade_version = Gem::Version.new(upgrade_version)
    file_ext = File.extname(@file_name)
    case file_ext
    when '.xlsx'
      @xml_doc = Nokogiri::XML::Document.new
      @intermediate_version = Gem::Version.new('4.17.0') # or earlier
    when '.xml'
      @xml_doc = XMLReader.new.read(@file_name)
      @intermediate_version = Gem::Version.new(@xml_doc.at('TextDatabase')['Version'])
    else
      fail "ERROR: Unsupported text database file extension: #{file_ext}"
    end
  end

  def run
    # For each version where an upgrade of the text database is required,
    # a code snippet, like the 4 lines below, must be added, including
    # implementation of the actual UpgradeTo_X_Y class.
    # Note! In case of versions where the text database doesn't change,
    # nothing has to be done. The text database will automatically be
    # updated with the new @upgrade_version.
    version_4_18 = Gem::Version.new('4.18.0')
    if @intermediate_version < version_4_18 && @upgrade_version >= version_4_18
      @xml_doc, @intermediate_version = UpgradeTo_4_18.new(@file_name).run
    end
    # Now @xml_doc contains 4.18.0 xml data

    version_4_19 = Gem::Version.new('4.19.0')
    if @intermediate_version < version_4_19 && @upgrade_version >= version_4_19
      @xml_doc, @intermediate_version = UpgradeTo_4_19.new(@xml_doc).run
    end
    # Now @xml_doc contains 4.19.0 xml data

    version_4_23 = Gem::Version.new('4.23.0')
    if @intermediate_version < version_4_23 && @upgrade_version >= version_4_23
      @xml_doc, @intermediate_version = UpgradeTo_4_23.new(@xml_doc).run
    end
    # Now @xml_doc contains 4.23.0 xml data

    if @xml_doc.at('//TextDatabase')
      @xml_doc.at('TextDatabase')['Version'] = @upgrade_version.version
      xml_file_name = @file_name.gsub(/\.xlsx$/, '.xml')
      XMLWriter.new.write(xml_file_name, @xml_doc)
    else
      fail "ERROR: Unsupported upgrade version: #{@upgrade_version.version}"
    end
  end
end

class UpgradeTo_4_23
  def initialize(xml_doc)
    @xml_doc = xml_doc
  end

  def run  
    # Add <IsVector> inside <Typography>
    @xml_doc.xpath("//Typography").each do |typography_node|
      if !typography_node.has_attribute?('IsVector')
        typography_node['IsVector'] = 'no'
      end
    end

    # Add <IsVector> inside <LanguageSetting>
    @xml_doc.xpath("//LanguageSetting").each do |language_setting_node|
      if !language_setting_node.has_attribute?('IsVector')
        language_setting_node['IsVector'] = 'no'
      end
    end
    
    return @xml_doc, Gem::Version.new('4.23.0')
  end
end

class UpgradeTo_4_19
  def initialize(xml_doc)
    @xml_doc = xml_doc
    @languages = []
    @typographies = []
    @texts = []
  end

  def run
    xml_doc = Nokogiri::XML::Document.new
    xml_doc.encoding = 'utf-8'

    create_data_structures

    # Create <TextDatabase>
    textdatabase_node = xml_doc.add_child(Nokogiri::XML::Node.new('TextDatabase', xml_doc))
    textdatabase_node['xmlns:xsi'] = 'http://www.w3.org/2001/XMLSchema-instance'
    textdatabase_node['xsi:noNamespaceSchemaLocation'] = 'texts.xsd'
    textdatabase_node['Version'] = @intermediate_version.to_s

    # Create <Languages> inside <TextDatabase>
    languages_node = textdatabase_node.add_child(Nokogiri::XML::Node.new('Languages', xml_doc))
    @languages.each do |language|
      # Add <Text> with required attributes inside <Texts>
      language_node = languages_node.add_child(Nokogiri::XML::Node.new('Language', xml_doc))
      language_node['Id'] = language
    end

    # Create <Texts> inside <TextDatabase>
    texts_node = textdatabase_node.add_child(Nokogiri::XML::Node.new('Texts', xml_doc))

    # Create <TextGroup> inside <Texts>
    text_group_node = texts_node.add_child(Nokogiri::XML::Node.new('TextGroup', xml_doc))
    text_group_node['Id'] = "Group1"
    @texts.each do |text|
      # Add <Text> with required attributes inside <Texts>
      text_node = text_group_node.add_child(Nokogiri::XML::Node.new('Text', xml_doc))
      text_node['Id'] = text.id
      text_node['TypographyId'] = text.typography_id
      text_node['Alignment'] = text.alignment
      text.translations.each do |translation|
        # Add <Translation> with required attribute
        translation_node = text_node.add_child(Nokogiri::XML::Node.new('Translation', xml_doc))
        translation_node['Language'] = translation.language
        # Add optional attribute
        translation_node['Alignment'] = translation.alignment if translation.alignment
        # Add actual text
        translation_node.add_child(Nokogiri::XML::Text.new(translation.text, xml_doc))
      end
    end

    # Add <Typographies> inside <TextDatabase>
    typographies_node = textdatabase_node.add_child(Nokogiri::XML::Node.new('Typographies', xml_doc))
    @typographies.each do |typography|
      # Add <Typography> with required attributes inside <Typographies>
      typography_node = typographies_node.add_child(Nokogiri::XML::Node.new('Typography', xml_doc))
      typography_node['Id'] = typography.id
      typography_node['Font'] = typography.font_file
      typography_node['Size'] = typography.font_size
      typography_node['Bpp'] = typography.bpp
      typography_node['Direction'] = typography.direction
      # Add optional attributes
      typography_node['FallbackCharacter'] = typography.fallback_character if typography.fallback_character
      typography_node['WildcardCharacters'] = typography.wildcard_characters if typography.wildcard_characters
      typography_node['WidgetWildcardCharacters'] = typography.widget_wildcard_characters if typography.widget_wildcard_characters
      typography_node['WildcardCharacterRanges'] = typography.wildcard_ranges if typography.wildcard_ranges
      typography_node['EllipsisCharacter'] = typography.ellipsis_character if typography.ellipsis_character
      # Add <LanguageSetting> with required attributes inside <Typography>
      typography.language_settings.each do |language_setting|
        language_setting_node = typography_node.add_child(Nokogiri::XML::Node.new('LanguageSetting', xml_doc))
        language_setting_node['Language'] = language_setting.language
        language_setting_node['Font'] = language_setting.font_file
        language_setting_node['Size'] = language_setting.font_size
        language_setting_node['Bpp'] = language_setting.bpp
        language_setting_node['Direction'] = language_setting.direction
        # Add optional attributes
        language_setting_node['FallbackCharacter'] = language_setting.fallback_character if language_setting.fallback_character
        language_setting_node['WildcardCharacters'] = language_setting.wildcard_characters if language_setting.wildcard_characters
        language_setting_node['WidgetWildcardCharacters'] = language_setting.widget_wildcard_characters if language_setting.widget_wildcard_characters
        language_setting_node['WildcardCharacterRanges'] = language_setting.wildcard_ranges if language_setting.wildcard_ranges
        language_setting_node['EllipsisCharacter'] = language_setting.ellipsis_character if language_setting.ellipsis_character
      end
    end

    return xml_doc, Gem::Version.new('4.19.0')
  end

private
  class Typography
    attr_reader :id, :font_file, :font_size, :bpp, :direction, :fallback_character, :wildcard_characters, :widget_wildcard_characters, :wildcard_ranges, :ellipsis_character, :language_settings, :text_ids
    attr_writer :id
    def initialize(id, font_file, font_size, bpp, direction, fallback_character, wildcard_characters, widget_wildcard_characters, wildcard_ranges, ellipsis_character)
      @id = id
      @font_file = font_file
      @font_size = font_size
      @bpp = bpp
      @direction = direction
      @fallback_character = fallback_character
      @wildcard_characters = wildcard_characters
      @widget_wildcard_characters = widget_wildcard_characters
      @wildcard_ranges = wildcard_ranges
      @ellipsis_character = ellipsis_character
      @direction = direction
      @language_settings = []
      @text_ids = []
    end

    def add_language_setting(setting)
      @language_settings.push(setting)
    end

    def add_text_id(id)
      @text_ids.push(id)
    end

    def ==(other)
      self.id == other.id &&
      self.font_file == other.font_file &&
      self.font_size == other.font_size &&
      self.bpp == other.bpp &&
      self.direction == other.direction &&
      self.fallback_character == other.fallback_character &&
      self.wildcard_characters == other.wildcard_characters &&
      self.widget_wildcard_characters == other.widget_wildcard_characters &&
      self.wildcard_ranges == other.wildcard_ranges &&
      self.ellipsis_character == other.ellipsis_character &&
      self.language_settings == other.language_settings
    end

    alias eql? ==

    class LanguageSetting
      attr_reader :language, :font_file, :font_size, :bpp, :direction, :fallback_character, :wildcard_characters, :widget_wildcard_characters, :wildcard_ranges, :ellipsis_character
      def initialize(language, font_file, font_size, bpp, direction, fallback_character, wildcard_characters, widget_wildcard_characters, wildcard_ranges, ellipsis_character)
        @language = language
        @font_file = font_file
        @font_size = font_size
        @bpp = bpp
        @direction = direction
        @fallback_character = fallback_character
        @wildcard_characters = wildcard_characters
        @widget_wildcard_characters = widget_wildcard_characters
        @wildcard_ranges = wildcard_ranges
        @ellipsis_character = ellipsis_character
      end

      def ==(other)
        self.language == other.language &&
        self.font_file == other.font_file &&
        self.font_size == other.font_size &&
        self.bpp == other.bpp &&
        self.direction == other.direction &&
        self.fallback_character == other.fallback_character &&
        self.wildcard_characters == other.wildcard_characters &&
        self.widget_wildcard_characters == other.widget_wildcard_characters &&
        self.wildcard_ranges == other.wildcard_ranges &&
        self.ellipsis_character == other.ellipsis_character
      end

      alias eql? ==
    end
  end

  class Text
    attr_reader :id, :typography_id, :alignment, :translations
    attr_writer :typography_id
    def initialize(id, typography_id, alignment)
      @id = id
      @typography_id = typography_id
      @alignment = alignment
      @translations = []
    end

    def add_translation(translation)
      @translations.push(translation)
    end

    class Translation
      attr_reader :language, :alignment, :text
      def initialize(language, alignment, text)
        @language = language
        @alignment = alignment
        @text = text
      end
    end
  end

  # Objective for create_data_structure:
  # Create data structures that fits the new xml layout and make it easier
  # to identify and resolve any upgrading conflicts.
  # How it is done:
  # Iterate text nodes and create data structures for @languages, @typographies and @texts.
  # Note! Unused typographies will bee added at the end.
  def create_data_structures
    @xml_doc.xpath('/TextDatabase/Texts/Text').each do |text_node|
      text_id = text_node['Id']
      typo_id = text_node['TypographyId']
      alignment = text_node['Alignment']
      typography_node = @xml_doc.xpath("/TextDatabase/Typographies/Typography[@Id=\"#{typo_id}\"]")
      font_file = typography_node.attr('Font'); font_file = font_file.value if font_file
      font_size = typography_node.attr('Size'); font_size = font_size.value if font_size
      bpp = typography_node.attr('Bpp'); bpp = bpp.value if bpp
      direction = text_node['Direction']
      fallback_character = typography_node.attr('FallbackCharacter'); fallback_character = fallback_character.value if fallback_character
      wildcard_characters = typography_node.attr('WildcardCharacters'); wildcard_characters = wildcard_characters.value if wildcard_characters
      widget_wildcard_characters = typography_node.attr('WidgetWildcardCharacters'); widget_wildcard_characters = widget_wildcard_characters.value if widget_wildcard_characters
      wildcard_ranges = typography_node.attr('WildcardCharacterRanges'); wildcard_ranges = wildcard_ranges.value if wildcard_ranges
      ellipsis_character = typography_node.attr('EllipsisCharacter'); ellipsis_character = ellipsis_character.value if ellipsis_character

      typography = Typography.new(typo_id, font_file, font_size, bpp, direction, fallback_character, wildcard_characters, widget_wildcard_characters, wildcard_ranges, ellipsis_character)
      typography.add_text_id(text_id)
      text = Text.new(text_id, typo_id, alignment)

      text_node.xpath('./Translation').each do |translation_node|
        language = translation_node['Language']
        alignment = translation_node['Alignment']
        typo_id = translation_node['TypographyId']
        typography_node = @xml_doc.xpath("/TextDatabase/Typographies/Typography[@Id=\"#{typo_id}\"]")
        font_file = typography_node.attr('Font'); font_file = font_file.value if font_file
        font_size = typography_node.attr('Size'); font_size = font_size.value if font_size
        bpp = typography_node.attr('Bpp'); bpp = bpp.value if bpp
        direction = translation_node['Direction']
        fallback_character = typography_node.attr('FallbackCharacter'); fallback_character = fallback_character.value if fallback_character
        wildcard_characters = typography_node.attr('WildcardCharacters'); wildcard_characters = wildcard_characters.value if wildcard_characters
        widget_wildcard_characters = typography_node.attr('WidgetWildcardCharacters'); widget_wildcard_characters = widget_wildcard_characters.value if widget_wildcard_characters
        wildcard_ranges = typography_node.attr('WildcardCharacterRanges'); wildcard_ranges = wildcard_ranges.value if wildcard_ranges
        ellipsis_character = typography_node.attr('EllipsisCharacter'); ellipsis_character = ellipsis_character.value if ellipsis_character
        # Only add language setting if translation specific typography or direction is present
        # Note! Default values are used if no specific values exists
        if typo_id || direction
          language_setting = Typography::LanguageSetting.new(
            language,
            font_file ? font_file : typography.font_file,
            font_size ? font_size : typography.font_size,
            bpp ? bpp : typography.bpp,
            direction ? direction : typography.direction,
            fallback_character ? fallback_character : typography.fallback_character,
            wildcard_characters ? wildcard_characters : typography.wildcard_characters,
            widget_wildcard_characters ? widget_wildcard_characters : typography.widget_wildcard_characters,
            wildcard_ranges ? wildcard_ranges : typography.wildcard_ranges,
            ellipsis_character ? ellipsis_character : typography.ellipsis_character,
          )
          typography.add_language_setting(language_setting)
        end
        text.add_translation(Text::Translation.new(language, alignment, translation_node.text))

        # Only add language if not included in @languages
        @languages.push(language) if !@languages.include?(language)
      end

      # Only add typography if not included in @typographies
      if @typographies.include?(typography)
        index = @typographies.index(typography)
        # Move text_ids from duplicate
        # Note! text_ids represents all the texts that uses this specific typography
        (@typographies[index].text_ids << typography.text_ids).flatten!
      else
        @typographies.push(typography)
      end

      @texts.push(text)
    end

    # Add unused typographies and give them direction "LTR"
    @xml_doc.xpath('/TextDatabase/Typographies/Typography').each do |typo_node|
      typo_id = typo_node['Id']
      if !@typographies.any? { |typography| typography.id == typo_id}
        @typographies.push(Typography.new(typo_id, typo_node['Font'], typo_node['Size'], typo_node['Bpp'], 'LTR', typo_node['FallbackCharacter'], typo_node['WildcardCharacters'], typo_node['WidgetWildcardCharacters'], typo_node['WildcardCharacterRanges'], typo_node['EllipsisCharacter']))
      end
    end

    resolve_any_typography_naming_conflict
  end

  def resolve_any_typography_naming_conflict
    used_typography_ids = @typographies.collect{|typography| typography.id}.uniq

    # Group typographies according to id and resolve naming conflicts
    @typographies.group_by {|typography| typography.id}.each do |id, group|
      group.sort! do |a,b|
        # Most used first, or LTR before RTL, or first id occurrence
        a.text_ids.length != b.text_ids.length ? b.text_ids.length <=> a.text_ids.length : a.direction != b.direction ? a.direction <=> b.direction : a.text_ids <=> b.text_ids
      end

      # More than one typography with the same id
      if group.length > 1
        puts "WARNING: Resolving typography naming conflict for #{id}!"
      end

      # Do not rename first element, it is already reserved in used_typography_ids
      group[1..].each do |typography|
        # Rename typography
        new_id = id
        if typography.direction != group[0].direction
          new_id = "#{new_id}_#{typography.direction}"
        end
        if used_typography_ids.include?(new_id)
          suffix = 'A'
          while used_typography_ids.include?("#{new_id}_#{suffix}")
            suffix.next!
          end
          new_id = "#{new_id}_#{suffix}"
        end
        typography_index = @typographies.index(typography)
        @typographies[typography_index].id = new_id

        # Update texts with new typography ids
        @typographies[typography_index].text_ids.each do |text_id|
          text_index = @texts.index {|text| text.id == text_id}
          @texts[text_index].typography_id = @typographies[typography_index].id
        end

        used_typography_ids.push(new_id)
      end
    end
  end
end

class UpgradeTo_4_18
  def initialize(xlsx_file_name)
    @typographies, @text_entries = TextDatabaseParser_4_17.new(xlsx_file_name).run
  end

  def run
    xml_doc = Nokogiri::XML::Document.new
    xml_doc.encoding = 'utf-8'

    # Create <TextDatabase>
    textdatabase_node = xml_doc.add_child(Nokogiri::XML::Node.new('TextDatabase', xml_doc))
    textdatabase_node['xmlns:xsi'] = 'http://www.w3.org/2001/XMLSchema-instance'
    textdatabase_node['xsi:noNamespaceSchemaLocation'] = 'texts.xsd'
    textdatabase_node['Version'] = @intermediate_version.to_s

    # Create <Texts> inside <TextDatabase>
    texts_node = textdatabase_node.add_child(Nokogiri::XML::Node.new('Texts', xml_doc))
    @text_entries.each do |entry|
      # Add <Text> with required attributes inside <Texts>
      text_node = texts_node.add_child(Nokogiri::XML::Node.new('Text', xml_doc))
      text_node['Id'] = entry.text_id
      text_node['TypographyId'] = entry.default_typography
      text_node['Alignment'] = entry.default_alignment.capitalize
      text_node['Direction'] = entry.default_direction
      @text_entries.entries.first.languages.each do |lang|
        # Add <Translation> with required attribute
        translation_node = text_node.add_child(Nokogiri::XML::Node.new('Translation', xml_doc))
        translation_node['Language'] = lang
        # Add optional attributes
        translation_node['TypographyId'] = entry.typographies[lang] if entry.typographies[lang]
        translation_node['Alignment'] = entry.alignments[lang].capitalize if entry.alignments[lang]
        translation_node['Direction'] = entry.directions[lang] if entry.directions[lang]
        # Add actual text
        translation_node.add_child(Nokogiri::XML::Text.new(entry.translation_in(lang).text, xml_doc))
      end
    end

    # Add <Typographies> inside <TextDatabase>
    typographies_node = textdatabase_node.add_child(Nokogiri::XML::Node.new('Typographies', xml_doc))
    @typographies.each do |typo|
      # Add <Typography> with required attributes inside <Typographies>
      typography_node = typographies_node.add_child(Nokogiri::XML::Node.new('Typography', xml_doc))
      typography_node['Id'] = typo.name
      typography_node['Font'] = typo.font_file
      typography_node['Size'] = typo.font_size
      typography_node['Bpp'] = typo.bpp
      # Add optional attributes
      typography_node['FallbackCharacter'] = typo.fallback_character if typo.fallback_character
      typography_node['WildcardCharacters'] = typo.wildcard_characters if typo.wildcard_characters
      typography_node['WidgetWildcardCharacters'] = typo.widget_wildcard_characters if typo.widget_wildcard_characters
      typography_node['WildcardCharacterRanges'] = typo.wildcard_ranges if typo.wildcard_ranges
      typography_node['EllipsisCharacter'] = typo.ellipsis_character if typo.ellipsis_character
    end

    return xml_doc, Gem::Version.new('4.18.0')
  end
end

