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
require 'fileutils'

class XMLWriter
  def write(file_name, xml_doc)
    # Convert newlines to "&#10;"
    xml_doc.xpath("//Text/Translation").each do |translation_node|
      translation_node.native_content = translation_node.encode_special_chars(translation_node.text).gsub("\n", '&&#10;')
    end

    # Create a formatter for nice output before writing to file
    content = xml_doc.to_xml
    content.gsub!('/>', ' />')

    if File.exist?(file_name)
      old_content = File.read(file_name)
      return if content == old_content
    end

    File.write(file_name, content)
  end
end
