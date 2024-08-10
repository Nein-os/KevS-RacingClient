#ifndef SERVICES_H
#define SERVICES_H

class IKevS_DataCollector;

void run_connection(IKevS_DataCollector *data, bool *run);
/*bool parseYaml(const char *data, const char* path, const char **val, int *len);
bool parseYamlInt(const char *yamlStr, const char *path, int *dest);
bool parseYamlStr(const char *yamlStr, const char *path, char *dest, int maxCount);
void processYAMLSessionString(const char* yaml);*/

#endif // SERVICES_H