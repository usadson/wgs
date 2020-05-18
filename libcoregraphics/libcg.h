/**
 * BSD-2-Clause
 *
 * Copyright (c) 2020 Tristan
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS  SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND  ANY  EXPRESS  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED  WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE  DISCLAIMED.  IN  NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE   FOR  ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR
 * CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT  NOT  LIMITED  TO,  PROCUREMENT  OF
 * SUBSTITUTE  GOODS  OR  SERVICES;  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION)  HOWEVER  CAUSED  AND  ON  ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT,  STRICT  LIABILITY,  OR  TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING  IN  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __LIBCG_H__
#define __LIBCG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/**
 * This file doesn't need this include, but it's just convenient for programs
 * that use this library.
 */
#include <GL/glew.h>

enum CGShutdownReason {
	CG_SR_DEBUG_ESCAPEKEY,
};

enum CGImageType {
	CG_IT_JPEG,
	CG_IT_PNG,
};

struct CGShaderInitData {
	const char	**attributes;
	size_t		 attributesCount;
	const char	*fragmentShaderFilePath;
	const char	*vertexShaderFilePath;
};

struct CGShaderData {
	GLuint		 fragmentShader;
	GLuint		 program;
	GLuint		 vertexShader;
};

struct CGMeshInitData {
	GLuint		 dimensions;
	GLuint		 vertexCount;
	GLfloat		*vertices;
	GLsizeiptr	 verticesSize;
};

struct CGMeshData {
	GLuint		 vao;
	GLuint		 vbo;
	GLsizei		 count;
};

struct CGImage {
	size_t		 height;
	GLuint		 texture;
	size_t		 width;
};

struct CGImageInitData {
	const char	*path;
	enum CGImageType type;
};

/* float parameter is the delta time */
typedef bool (*CGRenderFunc)(float);
typedef void (*CGShutdownFunc)(void);

/**
 * After CGInitialize the program may do some initialization work that can fail
 * before CGStart. Call this function to cleanup data generated by CGInitialize
 */
void
CGCleanError(void);

void
CGDeleteImage(struct CGImage *);

void
CGDeleteMesh(struct CGMeshData *);

void
CGDeleteShader(struct CGShaderData *);

/**
 * This function should be called before any other function of libcg, otherwise
 * undefined behavior may occur.
 */
bool
CGInitialize(void);

void
CGSetRenderFunc(CGRenderFunc);

void
CGSetShutdownFunc(CGShutdownFunc);

/**
 * Notify libcg that the program should go in shutdown mode soon.
 */
void
CGSetShutdown(enum CGShutdownReason);

bool
CGLoadImage(struct CGImage *, struct CGImageInitData *);

bool
CGLoadMesh(struct CGMeshData *, struct CGMeshInitData *);

bool
CGLoadShader(struct CGShaderData *, struct CGShaderInitData *);

int
CGStart(void);

#ifdef __cplusplus
}
#endif

#endif /* __LIBCG_H__ */
