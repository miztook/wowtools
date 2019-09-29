#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include "wowM2Struct.h"

class wowEnvironment;

class wowM2File
{
public:
	explicit wowM2File(const wowEnvironment* wowEnv);
	~wowM2File();

public:

private:
	const wowEnvironment* WowEnvironment;
};