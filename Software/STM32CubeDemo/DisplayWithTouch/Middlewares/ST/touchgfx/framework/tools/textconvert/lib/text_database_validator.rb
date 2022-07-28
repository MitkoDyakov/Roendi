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
require 'lib/sanitizer'
require 'lib/text_database_parser'
require 'lib/text_database_parser_4_17'
require 'lib/text_database_parser_4_18'
require 'lib/xml_reader'

class TextDatabaseValidator
  def validate(file_name)
    @typographies
    @text_entries
    file_ext = File.extname(file_name)
    if file_ext == '.xlsx'
      @typographies, @text_entries = TextDatabaseParser_4_17.new(file_name).run
    elsif file_ext == '.xml'
      xml_doc = XMLReader.new.read(file_name)
      if xml_doc.at("TextDatabase")
        version = xml_doc.at("TextDatabase")["Version"]
        case version
        when '4.18.0', '4.18.1'
          @typographies, @text_entries = TextDatabaseParser_4_18.new(xml_doc).run
        when '4.19.0', '4.19.1'
          language, @typographies, @text_entries = TextDatabaseParser.new(xml_doc).run
        else
          fail "ERROR: Unknown text database version: #{version}"
        end
      else
        fail "ERROR: Invalid text database: #{file_name}"
      end
    else
      fail "ERROR: Unsupported text database file extension: #{file_ext}"
    end
    Sanitizer.new(@text_entries, @typographies, nil).run
  end
end
