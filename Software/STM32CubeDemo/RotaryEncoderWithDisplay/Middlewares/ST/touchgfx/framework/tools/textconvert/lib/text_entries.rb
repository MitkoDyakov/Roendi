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
class TextEntries
  include Enumerable

  def initialize
    @entries = []
    @unicode_is_rtl = false
  end

  def each(&block)
    @entries.each(&block)
  end

  def remove(entry)
    @entries.delete(entry)
  end

  def add(entry)
    @entries.push(entry)
  end

  def empty?
    @entries.empty?
  end

  def different_typographies
    [ar,default]
  end

  def different_alignments
    [ar,default]
  end

  def languages_with_specific_settings
    @entries.collect { |entry| entry.typographies.keys + entry.alignments.keys + entry.directions.keys }.flatten.uniq
  end

  def remove_language(language)
    @entries.each { |entry| entry.remove_translation_in(language) }
  end

  def typographies
    @entries.map(&:default_typography).uniq
  end

  def entries
    @entries
  end

  def with_typography(typography)
    @entries.select { |entry| entry.typography == typography }
  end

  def text_id(text_id)
    @entries.find { |entry| entry.text_id == text_id }
  end

  def all_text_ids
    @entries.collect { |entry| entry.text_id }
  end

  def include?(text_entry)
    @entries.find { |entry| entry.text_id == text_entry.text_id || entry.cpp_text_id == text_entry.cpp_text_id }
  end

  def unicode_uses_rtl
    @unicode_is_rtl = true
  end

  def is_rtl
    @unicode_is_rtl || @entries.any? { |entry| entry.is_rtl }
  end
end

class TextEntry
  attr_reader :text_id
  attr_reader :typographies
  attr_reader :alignments
  attr_reader :directions
  attr_reader :default_typography
  attr_reader :default_alignment
  attr_reader :default_direction

  def initialize(text_id, default_typography, default_alignment, default_direction)
    @text_id = text_id
    @typographies = {}
    @alignments = {}
    @directions = {}
    @translations = {}

    # default typography
    @default_typography = default_typography

    # default alignment
    @default_alignment = get_alignment_as_string(default_alignment)

    # default direction
    @default_direction = get_direction_as_string(default_direction)
  end

  def add_typography(language, typography)
    @typographies[language] = typography
  end
  def add_alignment(language, alignment)
    @alignments[language] = alignment
  end
  def add_direction(language, direction)
    @directions[language] = direction
  end

  def add_translation(language, text)
    translation = Translation.new(text)
    @translations[language] = translation
  end

  def remove_translation_in(language)
    @translations.delete(language)
  end

  def translations
    @translations.values
  end

  def translation_in(language)
    @translations[language]
  end

  def translations_with_typography(typography)
    languages_with_typography = languages.select do |language|
      if @typographies[language].nil?
        @default_typography == typography
      else
        @typographies[language] == typography
      end
    end

    languages_with_typography.collect{ |language| translation_in(language) }
  end

  def languages
    @translations.keys
  end

  def number_of_substitutions_in(language)
    @translations[language].number_of_substitutions
  end

  def cpp_text_id
    cppify(text_id)
  end

  # includes the default typography
  def get_all_typographies
    @typographies.values.compact.insert(0, @default_typography)
  end

  # includes the default alignment
  def get_all_alignments_as_string
    @alignments.values.compact.collect{ |a| get_alignment_as_string(a) }.insert(0, @default_alignment)
  end

  # includes the default direction
  def get_all_directions_as_string
    @directions.values.compact.collect{ |a| get_direction_as_string(a) }.insert(0, @default_direction)
  end

  def is_rtl
    get_all_directions_as_string.any? { |dir| dir == 'RTL' }
  end

  private

  def get_alignment_as_string(a)
    a.to_s.empty? ? 'LEFT' : a.to_s.upcase
  end

  def get_direction_as_string(d)
    d.to_s.empty? ? 'LTR' : d.to_s.upcase
  end

  def cppify(text)
    t_type = "T_" + text.upcase

    # strip the keys for characters, that can not be used in C++
    t_type = t_type.to_ascii
    t_type.gsub!(" ", "_")
    t_type.gsub!(/[^0-9a-zA-Z_]/, '')
    t_type
  end
end

class Translation
  attr_reader :text
  def initialize(text)
    @text = text
  end
  def empty?
    @text.nil? || @text.empty?
  end
  def length
    @text.length
  end
  def number_of_substitutions
    to_cpp.count("\2")
  end
  def unicodes
    # Collect all unicodes and add a terminating zero, which is also part of the string
    @unicodes ||=
      begin
        numbers.map { |number| number.to_s.gsub(/\[|\]/,'').to_i } + [0]
      end
  end
  def to_cpp
    cpp_text = @text.gsub("\2", '') # Remove all existing placeholders
    regex = Regexp.new(/([^\\]|^)<(|.*?[^\\])>/) # Avoid matching \< and \>
    while cpp_text.match(regex)
      cpp_text.gsub!(regex, '\1'+"\2")
    end
    cpp_text.gsub('\\<', '<').gsub('\\>', '>') # Remove \ before < and >
  end
  private
  def numbers
    to_cpp.unpack('U*')
  end
end

class String
  def to_ascii
    # modernized version of http://craigjolicoeur.com/blog/ruby-iconv-to-the-rescue
    self.encode("ASCII", "UTF-8", :undef => :replace, :invalid => :replace, :replace => '').
      unpack('U*').select { |cp| cp < 127 }.pack('U*')
  end
end
