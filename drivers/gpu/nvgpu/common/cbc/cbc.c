/*
 * CBC
 *
 * Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */


#include <nvgpu/gk20a.h>
#include <nvgpu/cbc.h>
#include <nvgpu/dma.h>
#include <nvgpu/log.h>
#include <nvgpu/nvgpu_mem.h>
#include <nvgpu/comptags.h>

void nvgpu_cbc_remove_support(struct gk20a *g)
{
	struct nvgpu_cbc *cbc = g->cbc;

	nvgpu_log_fn(g, " ");

	if (cbc == NULL) {
		return;
	}

	if (nvgpu_mem_is_valid(&cbc->compbit_store.mem)) {
		nvgpu_dma_free(g, &cbc->compbit_store.mem);
		(void) memset(&cbc->compbit_store, 0,
			sizeof(struct compbit_store_desc));
	}
	gk20a_comptag_allocator_destroy(g, &cbc->comp_tags);

	nvgpu_kfree(g, cbc);
	g->cbc = NULL;
}

int nvgpu_cbc_init_support(struct gk20a *g)
{
	int err = 0;
	struct nvgpu_cbc *cbc = g->cbc;

	nvgpu_log_fn(g, " ");

	if (cbc == NULL) {
		cbc = nvgpu_kzalloc(g, sizeof(*cbc));
		if (cbc == NULL) {
			return -ENOMEM;
		}
		g->cbc = cbc;
		if (g->ops.cbc.alloc_comptags != NULL) {
			err = g->ops.cbc.alloc_comptags(g, g->cbc);
		}
	}

	if (g->ops.cbc.init != NULL) {
		g->ops.cbc.init(g, g->cbc);
	}

	return err;
}
