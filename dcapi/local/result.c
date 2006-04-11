#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dc_local.h"

/********************************************************************
 * Functions
 */

DC_Result *_DC_createResult(const char *wu_name)
{
	char *logicalName;
	GList *l;
	DC_Result *result;
	DC_PhysicalFile *file;
	FILE *f;

	result = g_new0(DC_Result, 1);
	result->wu = _DC_getWUByName(wu_name);
	if (!result->wu)
	{
		DC_log(LOG_ERR, "Received result for unknown WU %s", wu_name);
		g_free(result);
		return NULL;
	}

	for (l = result->wu->output_files; l; l = l->next)
	{
		logicalName = g_strdup_printf("%s/%s", result->wu->workdir, (char *)l->data);
		f = fopen(logicalName, "r");
		if (f != NULL) /* File exists */
		{
			file = _DC_createPhysicalFile((char *)l->data, result->wu->workdir);
			result->output_files = g_list_append(result->output_files, file);
			result->num_outputs++;
			fclose(f);
		}
		g_free(logicalName);
	}

	return result;
}

void _DC_destroyResult(DC_Result *result)
{
	while (result->output_files)
	{
		_DC_destroyPhysicalFile((_DC_PhysicalFile *)result->output_files->data);
		result->output_files = g_list_delete_link(result->output_files,
			result->output_files);
	}
	g_free(result);
}