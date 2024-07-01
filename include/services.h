#ifndef SERVICES_H
#define SERVICES_H

#include "datatypes/session.h"

enum yaml_state {
	space,
	key,
	keysep,
	value,
	newline
};

void run_iracing_connection(bool *run);
bool parseYaml(const char *data, const char* path, const char **val, int *len);
bool parseYamlInt(const char *yamlStr, const char *path, int *dest);
bool parseYamlStr(const char *yamlStr, const char *path, char *dest, int maxCount);
void processYAMLSessionString(const char* yaml);

#endif // SERVICES_H