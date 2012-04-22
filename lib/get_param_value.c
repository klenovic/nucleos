#include <nucleos/string.h>
#include <asm/setup.h>

/**
 * @brief Parse boot parameter value
 * @param params[in]  pointer to command line parameters
 * @param name[in]  key to look up (string)
 * @param value[out]  key value (string)
 * @return 1 on success otherwise 0
 */
int parse_bootparam_value(const char *params, const char *name, char *value)
{
	int i = 0;
	int start = 0;
	int end = 0;
	char *s1 = strstr(params, name);

	if (!s1)
		return 0;

	for (i = 0; i < strnlen(s1, COMMAND_LINE_SIZE - 1) + 1; i++) {
		if (s1[i] == '=')
			start = i + 1;

		if (start && (s1[i] == ' ' || s1[i] == '\0')) {
			end = i;
			break;
		}
	}

	memcpy(value, s1 + start, end - start);
	value[end - start] = '\0';

	return 1;
}
