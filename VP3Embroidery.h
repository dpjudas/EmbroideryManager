#pragma once

class File;
class VP3ColorBlock;
class VP3Thread;
class VP3ThreadColor;

class VP3Design
{
public:
	int32_t centerX = 0; // 0.001 mm
	int32_t centerY = 0; // 0.001 mm
	uint8_t unknown0 = 0;
	uint8_t unknown1 = 0;
	uint8_t unknown2 = 0;
	int32_t minX = 0; // 0.001 mm
	int32_t minY = 0; // 0.001 mm
	int32_t maxX = 0; // 0.001 mm
	int32_t maxY = 0; // 0.001 mm
	int32_t width = 0; // 0.001 mm
	int32_t height = 0; // 0.001 mm
	std::string settings;
	uint8_t unknown3 = 100;
	uint8_t unknown4 = 100;
	int32_t unknown5 = 4096;
	int32_t unknown6 = 0;
	int32_t unknown7 = 0;
	int32_t unknown8 = 4096;
	char xxPP[4] = { 120, 120, 80, 80 };
	uint8_t unknown9 = 1;
	uint8_t unknown10 = 0;
	std::string producer;
	std::vector<VP3Thread> threads;
};

enum class VP3Material : uint8_t
{
	metallicHolographic = 0x03,
	cotton = 0x04,
	rayon = 0x05,
	polyester = 0x06,
	silk = 0x0a
};

enum class VP3StitchType
{
	stitch,
	jump
};

class Vp3StitchPoint
{
public:
	VP3StitchType type = {};
	int dx = 0; // 0.1 mm
	int dy = 0; // 0.1 mm
};

class VP3StitchRun
{
public:
	int8_t scaleX;
	int8_t scaleY;
	uint8_t unknown;
	std::vector<Vp3StitchPoint> stitches;
};

class VP3Thread
{
public:
	int32_t startX = 0; // 0.001 mm
	int32_t startY = 0; // 0.001 mm
	std::vector<VP3ThreadColor> colors;
	uint8_t unknown0 = 0;
	VP3Material material;
	uint8_t threadWeight = 40; // km/kg
	std::string catalog;
	std::string description;
	std::string brand;
	int32_t totalDisplacementX = 0; // 0.001 mm
	int32_t totalDisplacementY = 0; // 0.001 mm
	VP3StitchRun stitchRun;
	uint8_t unknown1 = 0;
};

class VP3ThreadColor
{
public:
	uint8_t unknown0 = 0;
	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;
	uint8_t unknown1 = 0;
	uint8_t unknown2 = 0;
};

class VP3Embroidery
{
public:
	static std::unique_ptr<VP3Embroidery> load(const std::string& filename);

	std::string producer;
	std::string settings;

	int32_t minX = 0; // 0.001 mm
	int32_t minY = 0; // 0.001 mm
	int32_t maxX = 0; // 0.001 mm
	int32_t maxY = 0; // 0.001 mm

	uint32_t stictTimeCount = 0;
	uint16_t threadChangeCount = 0;

	uint8_t unknown1 = 0x0c;
	uint8_t unknown2 = 0;

	std::vector<VP3Design> designs;

private:
	static std::string readString(std::shared_ptr<File>& file);
	static std::string readString8(std::shared_ptr<File>& file);
};
