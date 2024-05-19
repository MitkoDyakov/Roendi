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
CollectorStruct = Struct.new(:text_entries, :languages, :typographies)

def get_getFont_name(typography)
  if typography.is_vector
    "getFont_vector_#{typography.cpp_name}_#{typography.font_size}"
  else
    "getFont_#{typography.cpp_name}_#{typography.font_size}_#{typography.bpp}bpp"
  end
end

def unicode_array_to_hex_offset_comment(array)
  strings = array.slice_after(0).to_a
  # Inject ([], 0) and on each interation insert
  #  * the substring
  #  * the offset
  #  * a flag for "last element"
  # in the [] array + update the offset for next substring
  # Return [0] which is the array (discarding the offset placed at [1] in the injected ([],0)
  strings.each_with_index.inject([Array.new,0]) { |(result, offset), (string, index)| [ result.push([string, offset, index==strings.length-1]), offset + string.length ] }[0]
end

def unicode_array_to_string(array)
  array.inject('') do |txt, unicode|
    if unicode==2
      txt << "<>"
    elsif unicode>=32 && unicode<127
      txt << unicode.chr
    elsif unicode==0
      return txt
    elsif unicode==13
      txt << '\r'
    elsif unicode==10
      txt << '\n'
    else
      txt << '?'
    end
    txt
  end
end

class StringCollector < CollectorStruct
  def run(remap_global)
    #collect all strings for sorting
    all_strings = Array.new(languages.count, Array.new)
    text_entries.each do |text_entry|
      text_entry.translations.each_with_index do |translation, lang_index|
        # Fold all languages into one entry if global remapping
        all_strings[remap_global ? 0 : lang_index] += [translation]
      end
    end
    # sort by: Same length => sort by text, otherwise reverse sort on length
    # [ 'Slide','Yes','Cover','None' ] => [ 'Cover', 'Slide', 'None', 'Yes' ]
    all_strings = all_strings.map do |strings|
      strings.sort{|x,y| x.length == y.length ? x.text <=> y.text : y.length <=> x.length }
    end

    #collect all string indeces, and add to characters array
    string_mapping = Array.new
    characters_lists = Array.new
    all_strings.each_with_index do |strings, lang_index|
      string_indices = Hash.new
      characters = Array.new
      strings.each do |translation|
        #lookup translation in hash
        #if not found, add to characters and insert index in hash for translation and all suffices
        #if found, do nothing
        unicodes = translation.unicodes # This includes a terminating zero character
        index = string_indices[unicodes]
        if !index
          new_index = characters.length
          #puts "new string: #{translation.to_cpp} index: #{new_index}"
          characters.concat(unicodes)
          for start in 0 .. unicodes.length-1
            sub_string = unicodes[start..-1]
            # if the substring is present, all shorter substrings are also present, so do not add again
            break if string_indices[sub_string]
            string_indices[sub_string] = (new_index + start)
          end
        else
          #puts "existing string: #{translation.to_cpp} index: #{index}"
        end
      end
      string_mapping[lang_index] = string_indices
      characters_lists[lang_index] = characters
    end
    [string_mapping, characters_lists]
  end
end
