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

#include <sys/stat.h>

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libcg.h"

/** Init Data */
GLfloat meshVertices[] = {
	-0.5f, -0.5f,
	 0.5f, -0.5f,
	 0.0f,  0.5f
};

struct CGMeshInitData meshInitData = {
	.dimensions = 2,
	.vertexCount = 3,
	.vertices = meshVertices,
	.verticesSize = sizeof(meshVertices)
};

static const char *shaderAttributes[] = { "position" };

struct CGShaderInitData shaderInitData = {
	.attributes = shaderAttributes,
	.attributesCount = sizeof(shaderAttributes) / sizeof(shaderAttributes[0]),
	.fragmentShaderFilePath = "res/fragment_shader.glsl",
	.vertexShaderFilePath = "res/vertex_shader.glsl"
};

/** Global variables **/
struct CGShaderData	shader;
struct CGMeshData	mesh;

bool
mainMenuRenderer(float deltaTime);

void
shutdownFunction(void);

int
main(void) {
	if (!CGInitialize()) {
		fputs("[Main] CGInitialize failed.\n", stderr);
		return EXIT_FAILURE;
	}

	if (!CGLoadShader(&shader, &shaderInitData)) {
		fputs("[Main] GCLoadShader failed.\n", stderr);
		CGCleanError();
		return EXIT_FAILURE;
	}

	if (!CGLoadMesh(&mesh, &meshInitData)) {
		fputs("[Main] CGLoadMesh failed.\n", stderr);
		CGDeleteShader(&shader);
		CGCleanError();
		return EXIT_FAILURE;
	}

	CGSetRenderFunc(mainMenuRenderer);
	CGSetShutdownFunc(shutdownFunction);

	return CGStart();
}

/* new shit */
bool
mainMenuRenderer(float deltaTime) {
	(void) deltaTime;

	glUseProgram(shader.program);
	glBindVertexArray(mesh.vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.count);

	return true;
}

void
shutdownFunction(void) {
	CGDeleteShader(&shader);
	CGDeleteMesh(&mesh);
}
