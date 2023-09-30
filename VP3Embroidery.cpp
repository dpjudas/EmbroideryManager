
#include "Precomp.h"
#include "VP3Embroidery.h"
#include "File.h"
#include "UTF16.h"

std::unique_ptr<VP3Embroidery> VP3Embroidery::load(const std::string& filename)
{
	auto embroidery = std::make_unique<VP3Embroidery>();

	auto file = File::open_existing(filename);

	char magic[6];
	file->read(magic, 6);
	if (memcmp(magic, "%vsm%", 6) != 0)
		throw std::runtime_error("Not a VP3 file");

	embroidery->producer = readString(file);

	static const char tag010[3] = { 0, 1, 0 };
	static const char tag020[3] = { 0, 2, 0 };
	static const char tag030[3] = { 0, 3, 0 };
	static const char tag050[3] = { 0, 5, 0 };

	char chunktag[3];
	uint32_t chunksize;

	file->read(chunktag, 3);
	if (memcmp(chunktag, tag020, 3) != 0)
		throw std::runtime_error("Expected 020 tag");
	chunksize = file->read_uint32_be();

	std::string settings = readString(file);

	embroidery->maxX = file->read_int32_be();
	embroidery->maxY = file->read_int32_be();
	embroidery->minX = file->read_int32_be();
	embroidery->minY = file->read_int32_be();

	embroidery->stictTimeCount = file->read_uint32_be();
	embroidery->threadChangeCount = file->read_uint16_be();
	embroidery->unknown1 = file->read_uint8();
	embroidery->unknown2 = file->read_uint8();

	int numDesigns = file->read_uint8();
	for (int i = 0; i < numDesigns; i++)
	{
		file->read(chunktag, 3);
		if (memcmp(chunktag, tag030, 3) != 0)
			throw std::runtime_error("Expected 030 tag");
		chunksize = file->read_uint32_be();

		VP3Design design;
		design.centerX = file->read_int32_be();
		design.centerY = file->read_int32_be();
		design.unknown0 = file->read_uint8(); // 0
		design.unknown1 = file->read_uint8(); // 0
		design.unknown2 = file->read_uint8(); // 0
		design.maxX = file->read_int32_be();
		design.maxY = file->read_int32_be();
		design.minX = file->read_int32_be();
		design.minY = file->read_int32_be();
		design.width = file->read_int32_be();
		design.height = file->read_int32_be();
		design.settings = readString(file);
		design.unknown3 = file->read_uint8(); // 100
		design.unknown4 = file->read_uint8(); // 100
		design.unknown5 = file->read_int32_be(); // 4096
		design.unknown6 = file->read_int32_be(); // 0
		design.unknown7 = file->read_int32_be(); // 0
		design.unknown8 = file->read_int32_be(); // 4096
		file->read(design.xxPP, 4);
		design.unknown9 = file->read_uint8(); // 1
		design.unknown10 = file->read_uint8(); // 0
		design.producer = readString(file);

		int numThreads = file->read_uint16_be();
		for (int j = 0; j < numThreads; j++)
		{
			file->read(chunktag, 3);
			if (memcmp(chunktag, tag050, 3) != 0)
				throw std::runtime_error("Expected 050 tag");
			chunksize = file->read_uint32_be();

			VP3Thread thread;
			thread.startX = file->read_int32_be();
			thread.startY = file->read_int32_be();

			int numColors = file->read_uint8();
			for (int l = 0; l < numColors; l++)
			{
				VP3ThreadColor color;
				color.unknown0 = file->read_uint8();
				color.red = file->read_uint8();
				color.green = file->read_uint8();
				color.blue = file->read_uint8();
				color.unknown1 = file->read_uint8();
				color.unknown2 = file->read_uint8();
				thread.colors.push_back(color);
			}

			thread.unknown0 = file->read_uint8();
			thread.material = (VP3Material)file->read_uint8();
			thread.threadWeight = file->read_uint8();
			thread.catalog = readString8(file);
			thread.description = readString8(file);
			thread.brand = readString8(file);
			thread.totalDisplacementX = file->read_int32_be();
			thread.totalDisplacementY = file->read_int32_be();

			file->read(chunktag, 3);
			if (memcmp(chunktag, tag010, 3) != 0)
				throw std::runtime_error("Expected 010 tag");
			chunksize = file->read_uint32_be();
			if (chunksize < 3 || chunksize > 0x100'0000)
				throw std::runtime_error("Invalid 010 chunk size");

			thread.stitchRun.scaleX = file->read_int8();
			thread.stitchRun.scaleY = file->read_int8();
			thread.stitchRun.unknown = file->read_uint8();

			std::vector<uint16_t> data((chunksize - 3) / sizeof(uint16_t));
			file->read(data.data(), data.size() * sizeof(uint16_t));
			for (uint16_t& v : data)
				v = _byteswap_ushort(v);

			size_t pos = 0;
			while (pos < data.size())
			{
				if ((data[pos] & 0xff00) == 0x8000)
				{
					int command = data[pos++] & 0xff;
					if (command == 1)
					{
						if (pos + 1 >= data.size())
							break;

						Vp3StitchPoint point;
						point.type = VP3StitchType::jump;
						point.dx = (int16_t)(data[pos]);
						point.dy = (int16_t)(data[pos + 1]);
						thread.stitchRun.stitches.push_back(point);
						pos += 2;
					}
				}
				else
				{
					Vp3StitchPoint point;
					point.type = VP3StitchType::stitch;
					point.dx = (int8_t)(data[pos] >> 8);
					point.dy = (int8_t)(data[pos] & 0xff);
					thread.stitchRun.stitches.push_back(point);
					pos++;
				}
			}

			thread.unknown1 = file->read_uint8();

			design.threads.push_back(std::move(thread));
		}

		embroidery->designs.push_back(std::move(design));
	}

	return embroidery;
}

std::string VP3Embroidery::readString(std::shared_ptr<File>& file)
{
	uint16_t bytelength = file->read_uint16_be();

	std::wstring str;
	str.resize((bytelength + 1) / 2);
	file->read(str.data(), bytelength);

	// Remove null terminators
	while (!str.empty() && str.back() == 0)
		str.pop_back();

	// Big endian to little endian
	for (wchar_t& c : str)
		c = _byteswap_ushort(c);
	
	return from_utf16(str);
}

std::string VP3Embroidery::readString8(std::shared_ptr<File>& file)
{
	uint16_t bytelength = file->read_uint16_be();

	std::string str;
	str.resize(bytelength);
	file->read(str.data(), bytelength);

	// Remove null terminators
	while (!str.empty() && str.back() == 0)
		str.pop_back();

	return str;
}
