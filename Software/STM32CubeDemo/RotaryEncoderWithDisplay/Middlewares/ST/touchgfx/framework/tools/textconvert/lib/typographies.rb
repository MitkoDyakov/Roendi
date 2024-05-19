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
class Typographies
  def initialize()
    @entries = {}
  end

  def add(default_typography, language, font_file, font_size, bpp, is_vector, fallback_character, ellipsis_character, wildcard_characters, widget_wildcard_characters, wildcard_ranges, direction)
    name = "#{default_typography}"
    name += "_AUTO_GENERATED_FOR_#{language}" if !language.empty?
    typography = Typography.new(name, font_file, font_size.to_i, bpp.to_i, is_vector, fallback_character, ellipsis_character, wildcard_characters, widget_wildcard_characters, wildcard_ranges)

    index = Index.new(default_typography, language)
    @entries[index] = {:typography => typography, :direction => direction}

    resolve_any_typography_naming_conflicts
  end

  def get_typographies
    @entries.values.map { |entry| entry[:typography] }
  end

  def get_typography_name(default_typography, language)
    index = lookup_index(default_typography, language)
    @entries[index][:typography].name
  end

  def get_direction(default_typography, language)
    index = lookup_index(default_typography, language)
    @entries[index][:direction]
  end

private
  def lookup_index(default_typography, language)
    # First lookup language specific, if not present, then lookup default
    index = Index.new(default_typography, language)
    if @entries[index].nil?
      index = Index.new(default_typography, "")
    end
    if @entries[index].nil?
      language_error_text = " with language setting for '#{language}'" if !language.empty?
      fail "ERROR: Unknown typography '#{default_typography}'#{language_error_text}"
    end
    index
  end

  def is_all_typography_names_uniq
    original = @entries.values.map { |entry| entry[:typography].name}
    original == original.uniq
  end

  def rename_typography_at(index)
    id = 1
    suffix = @entries[index][:typography].name.match(/_(\d+$)/)
    if !suffix.nil?
      @entries[index][:typography].name.delete_suffix!(suffix[0])
      id += suffix[1].to_i
    end
    @entries[index][:typography].name += "_#{id}"
  end

  def resolve_any_typography_naming_conflicts
    # Objective:
    #  Do not change default names
    #  Change language specific typography names if conflicts
    default_typographies = @entries.select { |index, entry| index.language.empty? }
    while !is_all_typography_names_uniq
      language_specific_typographies = @entries.select { |index, entry| !index.language.empty? }
      language_specific_typographies.each do |index, lang_spec_entry|
        if default_typographies.values.any? { |def_entry| def_entry[:typography].name == lang_spec_entry[:typography].name }
          # Rename language specific typography
          rename_typography_at(index)
        end
      end
    end
  end
end

class Typography < Struct.new(:name, :font_file, :font_size, :bpp, :is_vector, :fallback_character, :ellipsis_character, :wildcard_characters, :widget_wildcard_characters, :wildcard_ranges)
  def cpp_name
    font_file.gsub(/\.ttf$/,"").gsub(/[^0-9a-zA-Z]/, "_")
  end
end

# Inspired by class Book example: https://ruby-doc.org/core-3.1.0/Hash.html
class Index
  attr_reader :default_typography, :language

  def initialize(default_typography, language)
    @default_typography = default_typography
    @language = language
  end

  def ==(other)
    self.class === other &&
      other.default_typography == @default_typography &&
      other.language == @language
  end

  alias eql? ==

  def hash
    @default_typography.hash ^ @language.hash # XOR
  end
end
