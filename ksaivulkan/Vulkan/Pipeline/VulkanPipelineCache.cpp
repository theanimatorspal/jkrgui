#include "VulkanPipelineCache.hpp"
#include <iostream>

ksai::VulkanPipelineCache::VulkanPipelineCache(VulkanDevice &inDevice, std::string_view inFileName)
    : mName(inFileName), mDevice(inDevice.GetDeviceHandle()) {
    //   FILE *PtrReadFile = fopen(inFileName.c_str(), "rb");
    //   mPipelineCacheSize = GetFileSizeUtil(PtrReadFile);
    // mPipelineCacheData = (char*)malloc(sizeof(char) * mPipelineCacheSize);
    // if (mPipelineCacheData == nullptr)
    //{
    //	fputs("Memory error", stderr);
    //	exit(EXIT_FAILURE);
    //}

    // size_t Result = fread(mPipelineCacheData, 1, mPipelineCacheSize, PtrReadFile);
    // bool FileReadSuccessful = Result == mPipelineCacheSize;
    // if (!FileReadSuccessful)
    //{
    //	fputs("Reading Error", stderr);
    //	free(mPipelineCacheData);
    //	exit(EXIT_FAILURE);
    // }
    // if (PtrReadFile != nullptr)
    //{
    //	fclose(PtrReadFile);
    // }

    // CheckCacheData(inFileName);

    // auto CacheCreateInfo = vk::PipelineCacheCreateInfo(
    //	vk::PipelineCacheCreateFlags(),
    //	mPipelineCacheSize,
    //	mPipelineCacheData
    //);
    // mPipelineCache = mDevice.createPipelineCache(CacheCreateInfo);
    // free(mPipelineCacheData);
    // mPipelineCacheData = NULL;
}

ksai::VulkanPipelineCache::~VulkanPipelineCache() { mDevice.destroyPipelineCache(mPipelineCache); }

std::vector<uint8_t> ksai::VulkanPipelineCache::Get() {
    std::vector<unsigned char> PipelineCacheData;
    size_t CacheSize  = 0;
    vk::Result Result = mDevice.getPipelineCacheData(mPipelineCache, &CacheSize, nullptr); 
    if (Result != vk::Result::eSuccess) Log("IDK What PipelineCache", "ERROR");

    PipelineCacheData.resize(CacheSize);
    Result = mDevice.getPipelineCacheData(mPipelineCache, &CacheSize, PipelineCacheData.data());
    if (Result != vk::Result::eSuccess) Log("IDK What PipelineCache", "ERROR");
    // TODO Fix this
    return {};
}

void ksai::VulkanPipelineCache::CheckCacheData(std::string &inFileName) {
    if (mPipelineCacheData != nullptr) {
        uint32_t headerLength                   = 0;
        uint32_t cacheHeaderVersion             = 0;
        uint32_t vendorID                       = 0;
        uint32_t deviceID                       = 0;
        uint8_t pipelineCacheUUID[VK_UUID_SIZE] = {};
        memcpy(&headerLength, (uint8_t *)mPipelineCacheData + 0, 4);
        memcpy(&cacheHeaderVersion, (uint8_t *)mPipelineCacheData + 4, 4);
        memcpy(&vendorID, (uint8_t *)mPipelineCacheData + 8, 4);
        memcpy(&deviceID, (uint8_t *)mPipelineCacheData + 12, 4);
        memcpy(pipelineCacheUUID, (uint8_t *)mPipelineCacheData + 16, VK_UUID_SIZE);
        bool badCache = false;

        if (headerLength <= 0) {
            badCache = true;
            printf("  Bad header length in %s.\n", inFileName.c_str());
            printf("    Cache contains: 0x%.8x\n", headerLength);
        }

        if (cacheHeaderVersion != VK_PIPELINE_CACHE_HEADER_VERSION_ONE) {
            badCache = true;
            printf("  Unsupported cache header version in %s.\n", inFileName.c_str());
            printf("    Cache contains: 0x%.8x\n", cacheHeaderVersion);
        }

        // if (vendorID != info.gpu_props.vendorID) {
        //	badCache = true;
        //	printf("  Vendor ID mismatch in %s.\n", inFileName.c_str());
        //	printf("    Cache contains: 0x%.8x\n", vendorID);
        //	printf("    Driver expects: 0x%.8x\n", info.gpu_props.vendorID);
        // }

        // if (deviceID != info.gpu_props.deviceID) {
        //	badCache = true;
        //	printf("  Device ID mismatch in %s.\n", inFileName.c_str());
        //	printf("    Cache contains: 0x%.8x\n", deviceID);
        //	printf("    Driver expects: 0x%.8x\n", info.gpu_props.deviceID);
        // }

        // if (memcmp(pipelineCacheUUID, info.gpu_props.pipelineCacheUUID,
        // sizeof(pipelineCacheUUID)) != 0) { 	badCache = true; 	printf("  UUID mismatch in %s.\n",
        // inFileName.c_str()); 	printf("    Cache contains: "); 	printf(" PRint UUID HERE");
        //	printf("\n");
        //	printf("    Driver expects: ");
        //	printf(" Print UUID HERE");
        //	//print_UUID(info.gpu_props.pipelineCacheUUID);
        //	printf("\n");
        // }

        if (badCache) {
            // Don't submit initial cache data if any version info is incorrect
            free(mPipelineCacheData);
            mPipelineCacheSize = 0;
            mPipelineCacheData = nullptr;

            // And clear out the old cache file for use in next run
            printf("  Deleting cache entry %s to repopulate.\n", inFileName.c_str());
            // if (remove(inFileName.c_str()) != 0) {
            //	fputs("Reading error", stderr);
            //	exit(EXIT_FAILURE);
            // }
        }
    }
}

size_t ksai::VulkanPipelineCache::GetFileSizeUtil(FILE *inFile) {
    int CacheSize = 0;
    if (inFile) {
        fseek(inFile, 0, SEEK_END);
        CacheSize = ftell(inFile);
        rewind(inFile);
    }
    return CacheSize;
}
