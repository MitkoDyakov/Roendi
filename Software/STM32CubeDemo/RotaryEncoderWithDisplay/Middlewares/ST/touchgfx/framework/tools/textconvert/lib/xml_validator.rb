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

class XMLValidator
  def validate(xml_file_name)
    xml_doc = Nokogiri::XML(File.read(xml_file_name))
    schema_file_name = xml_file_name.gsub(/\.xml$/, '.xsd')
    if File.exists?(schema_file_name)
      xsd = Nokogiri::XML::Schema(File.read(schema_file_name))
      result = xsd.validate(xml_doc)
      if !result.empty?
        fail (["ERROR: Invalid \"#{xml_file_name}\""] + result.map { |err| "Line #{err.line}: #{err.message}" }) * "\n"
      end
    else
        fail "ERROR: Schema file \"#{schema_file_name}\" not found."
    end
  end
end

