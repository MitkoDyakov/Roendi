# Copyright (c) 2018(-2022) STMicroelectronics.
# All rights reserved.
#
# This file is part of the TouchGFX 4.19.1 distribution.
#
# This software is licensed under terms that can be found in the LICENSE file in
# the root directory of this software component.
# If no LICENSE file comes with this software, it is provided AS-IS.
#
###############################################################################/
require 'lib/outputter'
require 'lib/sanitizer'
require 'lib/string_collector'
require 'lib/text_database_parser'
require 'lib/xml_reader'
require 'lib/xml_validator'

class Generator
  def run(xml_file_name, output_path, text_output_path, font_asset_path, data_format, remap_identical_texts, autohint_setting, generate_binary_translations, generate_binary_fonts, framebuffer_bpp, generate_font_format)
    xml_doc = XMLReader.new.read(xml_file_name)
    XMLValidator.new.validate(xml_file_name)
    languages, typographies, text_entries = TextDatabaseParser.new(xml_doc).run
    Sanitizer.new(text_entries, typographies, framebuffer_bpp).run
    if remap_identical_texts=='yes'
      string_indices, characters = StringCollector.new(text_entries, typographies).run
    end
    Outputter.new(string_indices, characters, text_entries, typographies, text_output_path, output_path, font_asset_path, data_format, remap_identical_texts, autohint_setting, generate_binary_translations, generate_binary_fonts, generate_font_format).run
  end
end
