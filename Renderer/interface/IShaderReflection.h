/**
 *  Copyright 2020-2021 Zhang QiuLiang (juteman). All rights reserved.
 *
 *  This file is a part of Hawl
 *  see(https://github.com/juteman/Hawl)
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#pragma once
#include "BaseType.h"
#include "IRenderer.h"

static const uint32_t MAX_SHADER_STAGE_COUNT = 5;

typedef enum TextureDimension
{
	TEXTURE_DIM_1D,
	TEXTURE_DIM_2D,
	TEXTURE_DIM_2DMS,
	TEXTURE_DIM_3D,
	TEXTURE_DIM_CUBE,
	TEXTURE_DIM_1D_ARRAY,
	TEXTURE_DIM_2D_ARRAY,
	TEXTURE_DIM_2DMS_ARRAY,
	TEXTURE_DIM_CUBE_ARRAY,
	TEXTURE_DIM_COUNT,
	TEXTURE_DIM_UNDEFINED,
} TextureDimension;

struct VertexInput
{
	// The size of the attribute
	uint32_t size;

	// resource name
	const char* name;

	// name size
	uint32_t name_size;
};


struct ShaderResource
{
	// resource Type
	DescriptorType type;

	// The resource set for binding frequency
	uint32_t set;

	// The resource binding location
	uint32_t reg;

	// The size of the resource. This will be the DescriptorInfo array size for textures
	uint32_t size;

	// what stages use this resource
	ShaderStage used_stages;

	// resource name
	const char* name;

	// name size
	uint32_t name_size;

	// 1D / 2D / Array / MSAA / ...
	TextureDimension dim;

};

struct ShaderVariable
{
	// parents resource index
	uint32_t parent_index;

	// The offset of the Variable.
	uint32_t offset;

	// The size of the Variable.
	uint32_t size;

	// Variable name
	const char* name;

	// name size
	uint32_t name_size;

};

struct ShaderReflection
{
	ShaderStage mShaderStage;

	// single large allocation for names to reduce number of allocations
	char*    pNamePool;
	uint32_t mNamePoolSize;

	VertexInput* pVertexInputs;
	uint32_t     mVertexInputsCount;

	ShaderResource* pShaderResources;
	uint32_t        mShaderResourceCount;

	ShaderVariable* pVariables;
	uint32_t        mVariableCount;

	// Thread group size for compute shader
	uint32_t mNumThreadsPerGroup[3];

	//number of tessellation control point
	uint32_t mNumControlPoint;

#if defined(VULKAN)
	char* pEntryPoint;
#endif
};

struct PipelineReflection
{
	ShaderStage mShaderStages;
	// the individual stages reflection data.
	ShaderReflection mStageReflections[MAX_SHADER_STAGE_COUNT];
	uint32_t         mStageReflectionCount;

	uint32_t mVertexStageIndex;
	uint32_t mHullStageIndex;
	uint32_t mDomainStageIndex;
	uint32_t mGeometryStageIndex;
	uint32_t mPixelStageIndex;

	ShaderResource* pShaderResources;
	uint32_t        mShaderResourceCount;

	ShaderVariable* pVariables;
	uint32_t        mVariableCount;
};

void destroyShaderReflection(ShaderReflection* pReflection);

void createPipelineReflection(ShaderReflection* pReflection, uint32_t stageCount, PipelineReflection* pOutReflection);
void destroyPipelineReflection(PipelineReflection* pReflection);
