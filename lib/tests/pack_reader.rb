#!/usr/bin/env ruby

require 'zlib'


class Fixnum
  def to_hex_s
    '0x' + to_s(16)
  end
end

class String
  def to_hex_s
    unpack("H2"*size).join
  end
  alias_method :to_sha1_s, :to_hex_s

  def to_offset_i
    unpack("N")[0]
  end
  alias_method :to_version_i, :to_offset_i
  alias_method :to_crc_i, :to_offset_i
end


PACK_SIGNATURE = "PACK"
IDX_SIGNATURE = "\377tOc"

ObjectTypes = [:none, :commit, :tree, :blob, :tag, nil, :offset_delta, :ref_delta]

CrcSize          =   4
FanOutCount      = 256
IndexOffsetSize  =   4
PackEntryCountSize = 4
Sha1Size        =   20
SignatureSize   =    4
VersionSize     =    4



def all_crcs
  crcs = []
  with_idx do |idx|
    case @index_version
    when 2
      idx.seek(@crc_table[:start])
      @index_entry_count.times do
        crcs << idx.read(CrcSize).to_crc_i
      end
    else
      return nil
    end
  end
  crcs
end

def all_ids
  ids = []
  with_idx do |idx|
    case @index_version
    when 2
      idx.seek(@sha1_table[:start])
      @index_entry_count.times do
        ids << idx.read(Sha1Size).to_sha1_s
      end
    else
      idx.seek(@offset_table[:start])
      @index_entry_count.times do
        idx.read(IndexOffsetSize) # skip offset
        ids << idx.read(Sha1Size).to_sha1_s
      end
    end
  end
  ids
end

def all_offsets
  offsets = []
  with_idx do |idx|
    case @index_version
    when 2
      idx.seek(@offset_table[:start])
      @index_entry_count.times do
        offsets << idx.read(IndexOffsetSize).to_offset_i
      end
    else
      idx.seek(@offset_table[:start])
      @index_entry_count.times do
        offsets << idx.read(IndexOffsetSize).to_offset_i
        idx.read(Sha1Size) # skip sha1
      end
    end
  end
  offsets
end

def size_from_delta_header(delta, pos)
  size = 0
  shift = 0
  begin
    c = delta[pos]
    if c == nil
      raise 'invalid delta header'
    end
    pos += 1
    size |= (c & 0x7f) << shift
    shift += 7
  end while c & 0x80 != 0
  [size, pos]
end

def id_at(offset)
  ids = all_ids
  all_offsets.each_with_index do |ofs, i|
    return ids[i] if ofs == offset
  end

  raise "no id for offset #{"0x%08x"%offset} found"
end

def init_index()
  with_idx do |idx|
    signature = idx.read(SignatureSize)
    version = idx.read(VersionSize).to_version_i
    if signature == IDX_SIGNATURE
      raise "Unknown index version #{version}" if version > 2
      @index_version = version
      @index_header_size = 8
    else
      @index_version = 1
      @index_header_size = 0
    end

    @fan_out_table = {}
    @fan_out_table[:start] = @index_header_size
    @fan_out_table[:entry_size] = IndexOffsetSize
    @fan_out_table[:entries] = FanOutCount
    @fan_out_table[:size] = @fan_out_table[:entries] * @fan_out_table[:entry_size]

    idx.seek(@fan_out_table[:start] + @fan_out_table[:size] - @fan_out_table[:entry_size])
    @index_entry_count = idx.read(@fan_out_table[:entry_size]).to_offset_i
  end

  @offset_table = {}
  @sha1_table = nil
  @crc_table = nil
  case(@index_version)
  when 2
    @sha1_table = {}
    @sha1_table[:start] = @fan_out_table[:start] + @fan_out_table[:size]
    @sha1_table[:entry_size] = Sha1Size
    @sha1_table[:entries] = @index_entry_count
    @sha1_table[:size] = @sha1_table[:entries] * @sha1_table[:entry_size]

    @crc_table = {}
    @crc_table[:start] = @sha1_table[:start] + @sha1_table[:size]
    @crc_table[:entry_size] = CrcSize
    @crc_table[:entries] = @index_entry_count
    @crc_table[:size] = @crc_table[:entries] * @crc_table[:entry_size]

    @offset_table[:start] = @crc_table[:start] + @crc_table[:size]
    @offset_table[:entry_size] = IndexOffsetSize
    @offset_table[:entries] = @index_entry_count
    @offset_table[:size] = @offset_table[:entries] * @offset_table[:entry_size]
  else
    @offset_table[:start] = @fan_out_table[:start] + @fan_out_table[:size]
    @offset_table[:entry_size] = IndexOffsetSize + Sha1Size
    @offset_table[:entries] = @index_entry_count
    @offset_table[:size] = @offset_table[:entries] * @offset_table[:entry_size]
  end

  with_idx do |idx|
    @index_trailer = {}
    @index_trailer[:start] = @offset_table[:start] + @offset_table[:size]
    idx.seek(@index_trailer[:start])
    @index_trailer[ :pack_checksum] = idx.read(Sha1Size).to_sha1_s
    @index_trailer[:index_checksum] = idx.read(Sha1Size).to_sha1_s
  end
end

def init_pack()
  with_pack do |pack|
    signature = pack.read(SignatureSize)
    @pack_version = pack.read(VersionSize).to_offset_i
    @pack_entry_count = pack.read(PackEntryCountSize).to_offset_i
  end
end

def offset_for(id)
  all_ids.each_with_index do |sha1, i|
    return all_offsets[i] if sha1.start_with?(id)
  end

  raise "id #{id} not found"
end

def pack_data_at(offset)
  with_pack do |pack|
    pack.seek(offset)

    c = pack.read(1)[0]
    dest_size = c & 0xf;
    type = ObjectTypes[(c >> 4) & 7]
    shift = 4
    data_offset = offset+1;

    while (c & 0x80 != 0)
      c = pack.read(1)[0]
      dest_size |= ((c & 0x7f) << shift)
      shift += 7
      data_offset += 1
    end

    case(type)
    when :offset_delta, :ref_delta
      delta_data = pack.read(Sha1Size)
      if type == :offset_delta
        i = 0
        c = delta_data[i]
        offset_to_base = c & 0x7f
        while c & 0x80 != 0
          i += 1
          c = delta_data[i]
          offset_to_base += 1
          offset_to_base <<= 7
          offset_to_base |= c & 0x7f
        end
        base_offset = offset - offset_to_base
        delta_data_offset = data_offset + i + 1
        base_data = pack_data_at(base_offset)[:data]
        base_size = pack_data_at(base_offset)[:dest_size]
        pack.seek(delta_data_offset)

        delta_data = { :raw => Zlib::Inflate.inflate(pack.read(base_size)), :parsed => [] }
        pos = 0
        delta_src_size, pos = size_from_delta_header(delta_data[:raw], pos)
        delta_dest_size, pos = size_from_delta_header(delta_data[:raw], pos)
        delta_data[:src_size] = delta_src_size
        delta_data[:dest_size] = delta_dest_size

        dest_data = ""
        while pos < delta_data[:raw].size
          cmd_offset = pos
          c = delta_data[:raw][pos]
          pos += 1
          if c & 0x80 != 0
            pos -= 1
            cp_off = cp_size = 0
            cp_off = delta_data[:raw][pos += 1] if c & 0x01 != 0
            cp_off |= delta_data[:raw][pos += 1] << 8 if c & 0x02 != 0
            cp_off |= delta_data[:raw][pos += 1] << 16 if c & 0x04 != 0
            cp_off |= delta_data[:raw][pos += 1] << 24 if c & 0x08 != 0
            cp_size = delta_data[:raw][pos += 1] if c & 0x10 != 0
            cp_size |= delta_data[:raw][pos += 1] << 8 if c & 0x20 != 0
            cp_size |= delta_data[:raw][pos += 1] << 16 if c & 0x40 != 0
            cp_size = 0x10000 if cp_size == 0
            pos += 1
            dest_data += base_data[cp_off,cp_size]
            delta_data[:parsed] << {:cmd => :from_src, :offset => cp_off, :size => cp_size, :cmd_offset => cmd_offset}
          elsif c != 0
            dest_data += delta_data[:raw][pos,c]
            delta_data[:parsed] << {:cmd => :from_delta, :offset => pos, :size => c, :cmd_offset => cmd_offset}
            pos += c
          else
            raise 'invalid delta data'
          end
        end
        dest_data

        delta = {:offset_to_base => offset_to_base, :base_offset => base_offset, :data_offset => delta_data_offset, :data => delta_data, :dest_data => dest_data}
      else
        base_sha1 = delta_data.to_hex_s
        delta_data_offset += Sha1Size
        delta_data = Zlib::Inflate.inflate(pack.read(dest_size))
        delta = {:base_id => base_sha1, :data_offset => delta_data_offset, :data => delta_data}
      end
      return {:type => type, :dest_size => dest_size, :data_offset => data_offset, :delta => delta}
    when :commit, :tree, :blob, :tag
      data = Zlib::Inflate.inflate(pack.read(dest_size))
      return {:type => type, :dest_size => dest_size, :data_offset => data_offset, :data => data}
    else
      raise "Unknown object type #{type.inspect}"
    end
  end
end

def packed_size_for(id)
  offset = offset_for(id)
  offsets = all_offsets.sort
  offsets.sort.each_with_index do |ofs, i|
    if ofs == offset
      unless i+1 == offsets.size
        next_offset = offsets[i+1]
      else
        next_offset = File.stat("#{@pack_name}.pack").size - Sha1Size
      end
      return next_offset - offset
    end
  end
end

def print_all_ids
  all_ids.each_with_index do |sha1, i|
    puts "#{i} #{sha1}"
  end
end

def print_all_objects
  ids = all_ids
  offsets = all_offsets
  case(@index_version)
  when 2
    crcs = all_crcs
    puts "Id  Sha1                                     Offset     Crc"
    @index_entry_count.times do |i|
      puts "#{"%03d"%i} #{ids[i]} #{"0x%08x"%offsets[i]} #{"0x%08x"%crcs[i]}"
    end
  else
    puts "Id  Sha1                                     Offset"
    @index_entry_count.times do |i|
      puts "#{"%03d"%i} #{ids[i]} #{"0x%08x"%offsets[i]}"
    end
  end
end

def print_index_info
  puts "info -------------------------"
  puts "   version: #{@index_version}"
  puts "   entries: #{@index_entry_count}"

  case(@index_version)
  when 2
    puts "    tables: fan_out, sha1, crc, offset"
    puts "table offsets --------------"
    puts "  fan out table offset: #{"0x%08x"%@fan_out_table[:start]}"
    puts "     sha1 table offset: #{"0x%08x"%@sha1_table[:start]}"
    puts "      crc table offset: #{"0x%08x"%@crc_table[:start]}"
    puts "   offset table offset: #{"0x%08x"%@offset_table[:start]}"
  else
    puts "    tables: fan_out, offset"
    puts "table offsets --------------"
    puts "  fan_out table offset: #{"0x%08x"%@fan_out_table[:start]}"
    puts "  offset  table offset: #{"0x%08x"%@offset_table[:start]}"
  end
  puts "        trailer offset: #{"0x%08x"%@index_trailer[:start]}"

  puts "checksums --------------------"
  puts "    pack checksum sha1: #{@index_trailer[:pack_checksum]}"
  puts "   index checksum sha1: #{@index_trailer[:index_checksum]}"
end

def print_pack_data_at
  offset = ARGV.shift
  if offset.is_a?(String) && offset.start_with?("0x")
    offset = offset.hex
  else
    offset = offset.to_i
  end
  id = id_at(offset)
  pack_data = pack_data_at(offset)

  puts "info -------------------------"
  puts "             sha1: #{id}"
  puts "    object offset: #{"0x%08x"%offset}"
  puts "      data offset: #{"0x%08x"%pack_data[:data_offset]}"
  puts "             type: #{pack_data[:type]}"
  puts "  packed obj size: #{packed_size_for(id)}"
  puts "    dest obj size: #{pack_data[:dest_size]}"

  case(pack_data[:type])
  when :commit, :tree, :blob, :tag
    puts "data -------------------------"
    puts pack_data[:data]
  else
    delta_data = pack_data[:delta]
    src_data = pack_data_at(delta_data[:base_offset])[:data]
    puts "delta ------------------------"
    if :index_delta
      puts "      base offset: #{"0x%08x"%delta_data[:base_offset]} = #{"0x%08x"%offset} - #{"0x%08x"%delta_data[:offset_to_base]}"
    else
      puts "          base id: #{delta_data[:base_id]}"
    end
      puts "     src obj size: #{delta_data[:data][:src_size]}"
      puts "    dest obj size: #{delta_data[:data][:dest_size]}"
    puts "raw delta data ---------------"
    puts delta_data[:data][:raw].to_hex_s
    puts "parsed delta data ------------"
    delta_data[:data][:parsed].each do |cp|
      if cp[:cmd] == :from_src
        puts "  At #{"0x%08x"%cp[:cmd_offset]} in delta copy from source"
        puts "    offset: #{"0x%08x"%cp[:offset]}"
        puts "      size: #{cp[:size]}"
        puts "      data: #{src_data[cp[:offset],cp[:size]].inspect}"
      else
        puts "  At #{"0x%08x"%cp[:cmd_offset]} in delta copy from delta"
        puts "    offset: #{"0x%08x"%cp[:offset]}"
        puts "      size: #{cp[:size]}"
        puts "      data: #{delta_data[:data][:raw][cp[:offset],cp[:size]].inspect}"
      end
    end
    puts "dest data --------------------"
    puts delta_data[:dest_data]
  end
end

def print_pack_data_for
  id = ARGV.shift
  offset = offset_for(id)
  ARGV.unshift(offset)
  print_pack_data_at
end

def print_usage
  puts "Usage: #{$0} pack-012345 <command> [<option1> [option2 [...]]]"
  puts "Possible commands:"
  puts "\tall_ids"
  puts "\tall_objects"
  puts "\tindex_info"
  puts "\tpack_data_at <offset>"
  puts "\tpack_data_for <sha1>"
end

def with_idx
  idx = File.open("#{@pack_name}.idx", 'rb')
  yield idx
  idx.close
end

def with_pack
  pack = File.open("#{@pack_name}.pack", 'rb')
  yield pack
  pack.close
end



def main
  @pack_name = ARGV.shift
  @command = ARGV.shift

  init_index
  init_pack

  case(@command)
  when "all_ids"
    print_all_ids
  when "all_objects"
    print_all_objects
  when "index_info"
    print_index_info
  when "pack_data_at"
    print_pack_data_at
  when "pack_data_for"
    print_pack_data_for
  when nil, "-h", "--help", "help"
    print_usage
  else
    puts "unknown command #{@command.inspect}"
    print_usage
  end
end

main