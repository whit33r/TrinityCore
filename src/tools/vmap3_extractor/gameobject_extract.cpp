#include "model.h"
#include "dbcfile.h"
#include "adtfile.h"
#include "vmapexport.h"

#include <algorithm>

void ExtractGameobjectModels()
{
    printf("Extracting GameObject models...");
    DBCFile dbc("DBFilesClient\\GameObjectDisplayInfo.dbc");
    if(!dbc.open())
    {
        printf("Fatal error: Invalid GameObjectDisplayInfo.dbc file format!\n");
        exit(1);
    }

    std::string basepath = szWorkDirWmo;
    basepath += "/";
    std::string path;

    FILE * model_list = fopen((basepath + "temp_gameobject_models").c_str(), "wb");
    if (!model_list)
        return;

    for (DBCFile::Iterator it = dbc.begin(); it != dbc.end(); ++it)
    {
        path = it->getString(1);

        if (path.empty())
            continue;

//         fixnamen((char*)path.c_str(), path.size());
        char * name = GetPlainName((char*)path.c_str());
        fixname2(name, strlen(name));

        // < 3.1.0 ADT MMDX section store filename.mdx filenames for corresponded .m2 file
        std::string ext3 = path.size() >= 4 ? path.substr(path.size()-4,4) : "";
        std::transform( ext3.begin(), ext3.end(), ext3.begin(), ::tolower );
        if(ext3 == ".mdx")
        {
            // replace .mdx -> .m2
            path.erase(path.length()-2,2);
            path.append("2");
        }
        // >= 3.1.0 ADT MMDX section store filename.m2 filenames for corresponded .m2 file
        // nothing do
        else if (ext3 == ".mdl")
        {
            // TODO: convert mdl into mdx
            continue;
        }
        else
            continue;

        // for test
/*
        if (ext3 == ".mdl")
        {
            MPQFile mdl(path.c_str());
            if (mdl.isEof())
                continue;

            if (FILE* f = fopen((basepath + name).c_str(), "wb"))
            {
                fwrite(f, 1, mdl.getSize(), f);
                fclose(f);
            }
            continue;
        }
*/
        Model mdl(path);
        if (mdl.open())
        {
            path = basepath + name;
            mdl.ConvertToVMAPModel(path.c_str());

            uint32 path_length = strlen(name);
            fwrite(&path_length, sizeof(uint32), 1, model_list);
            fwrite(name, sizeof(char), path_length, model_list);
        }
    }

    fclose(model_list);

    printf("Done!\n");
}