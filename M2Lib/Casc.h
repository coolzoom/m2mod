#pragma once

#include "BaseTypes.h"
#include <string>
#include <map>

namespace M2Lib
{
	class Casc
	{
	private:
		static UInt32 Magic;

		bool LoadListFileCache();
		void ClearCache() { filesByFileDataId.clear(); fileDataIdsByHash.clear(); cacheLoaded = false; }

		std::string StoragePath;
		std::string BinListFile;
		std::string ListFile;

		void* hStorage = NULL;

		bool cacheLoaded = false;
		std::map<UInt32, std::string> filesByFileDataId;
		std::map<UInt64, UInt32> fileDataIdsByHash;

	public:
		Casc();

		void SetStoragePath(std::string const& StoragePath);

		bool Load();
		void Unload();
		bool IsLoaded() const { return hStorage != NULL; }
		bool GenerateListFileCache();
		UInt32 GetCacheSize() const { return filesByFileDataId.size(); }

		std::string GetFileByFileDataId(UInt32 FileDataId);
		UInt32 GetFileDataIdByFile(std::string const& File);
	};
}
