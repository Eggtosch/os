#!/usr/bin/ruby

def dir_children(dir)
	Dir.children(dir).map{File.join(dir, _1)}
end

if ARGV.empty? then
	STDERR.puts "Usage: #{$0} <font_folder> <output_header_file>"
	exit 1
end

if not File.directory?(ARGV[0]) or not File.exist?(ARGV[0]) then
	STDERR.puts "[#{$0}] File #{ARGV[0]} does not exist"
	exit 1
end

begin
	if dir_children(ARGV[0]).map{File.exist?(ARGV[1]) && File.mtime(_1) < File.mtime(ARGV[1])}.all? then
		STDOUT.puts "Files in #{ARGV[0]} have not changed"
		exit 0
	end
rescue
end

def read_txt(filepath, fname)
	font = File.read(filepath)
	font_lines = font.split("\n")
	width, height = font_lines[0].split("x").map{_1.to_i}

	font_lines = font_lines[1..]

	line_num = 2
	chars = {}

	font_lines.each_slice(2+height) do |lines|
		char = lines[0][0]

		chars[char] = []

		# check top outline for character
		if not lines[0].end_with?("-" * width + "+") then
			puts "[#{$0}] Error, wrong format at line #{line_num} in file #{filepath}"
			exit 1
		end
		line_num += 1

		lines[1..-2].each do |line|
			if not line[0] == "|" then
				STDERR.puts "[#{$0}] Error, wrong format at line #{line_num} in file #{filepath}"
				exit
			end
			# parse middle of character and write to outfile
			val = 0
			8.times do |i|
				val <<= 1
				val |= line[1+i] == 'X' ? 1 : 0
			end
			chars[char] << val
			if not line[-1] == "|" then
				STDERR.puts "[#{$0}] Error, wrong format at line #{line_num} in file #{filepath}"
				exit
			end
			line_num += 1
		end

		# check bottom outline for character
		if lines[-1] != "+#{"-" * width}+" then
			STDERR.puts "[#{$0}] Error, wrong format at line #{line_num} in file #{filepath}"
			exit 1
		end
		line_num += 1
	end
	fname = fname.split(".")[..-2].join(".")
	{name: fname, width: width, height: height, nglyphs: 256, data: chars}
end

def read_pgm(filepath, fname)
	f = File.open(filepath, "rb")
	font = f.read
	f.close
	font_lines = font.split("\n")
	width, height = font_lines[1].split(" ").map(&:to_i)
	data = font_lines[3]

	chars = {}
	curr  = ' '.ord

	size = width * height
	n = 0
	while n < width do
		chars[curr.chr] = []
		height.times do |y|
			idx = y * width + n
			byte = data[idx..(idx+7)]
			val = 0
			8.times do |i|
				val <<= 1
				val |= byte[i].unpack("C").first == 0xff ? 1 : 0
			end
			chars[curr.chr] << val
		end
		curr += 1
		n += 8
	end

	fname = fname.split(".")[..-2].join(".")
	{name: fname, width: 8, height: height, nglyphs: 256, data: chars}
end

fonts = []
Dir.children(ARGV[0]).each do |fname|
	filepath = File.join(ARGV[0], fname)
	if filepath.end_with?(".txt") then
		fonts << read_txt(filepath, fname)
	elsif filepath.end_with?(".pgm") then
		fonts << read_pgm(filepath, fname)
	else
		STDERR.puts "[#{$0}] Warning, file ending of file #{filepath} not supported, won't convert"
	end
end

template = %{/*
 * This header was auto generated from the folder %s
 * Date: %s
 */

#pragma once

struct font {
	const char *font_name;
	u16 font_width;
	u16 font_height;
	u16 font_nglyphs;
	u8 *font_data;
};

}
output_header = File.open(ARGV[1], "w+")
output_header.write(template % [File.expand_path(ARGV[0]), Time.now.strftime("%d-%m-%Y %H:%M:%S")])

def write_data(output_header, font, n)
	output_header.write("static u8 font%s[%s][%s] = {\n" % [n, font[:nglyphs], font[:height]])
	output_header.write(font[:data].map do |char, values|
		char = "\\'" if char == "'"
		char = "\\\\" if char == '\\'
		"['#{char}'] = {#{values.map{"0x%02x" % _1}.join(",")}}"
	end.join(",\n"))
	output_header.write("\n};\n\n")
end

fonts.each_with_index do |font, n|
	write_data(output_header, font, n)
end

output_header.write("static struct font fonts[] = {\n")
fonts.each_with_index do |font, n|
	name    = font[:name].inspect
	width   = font[:width]
	height  = font[:height]
	nglyphs = font[:nglyphs]
	table   = "font%s" % n
	output_header.write("\t{#{name}, #{width}, #{height}, #{nglyphs}, (u8*)#{table}},\n")
end
output_header.write("\t{NULL, 0, 0, 0, NULL}\n")
output_header.write("};\n")
