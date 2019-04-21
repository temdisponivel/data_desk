#include "data_desk.h"

typedef struct Entity
{
    char *name;
    char *name_lowercase_with_underscores;
    char *name_uppercase_with_underscores;
    char *name_upper_camel_case;
    char *name_lower_camel_case;
    DataDeskASTNode *root;
}
Entity;

static int global_entity_count = 0;
static Entity global_entities[1024];
static FILE *global_entity_file;

DATA_DESK_FUNC void
DataDeskCustomInitCallback(void)
{
    global_entity_file = fopen("generated_entity.c", "w");
}

DATA_DESK_FUNC void
DataDeskCustomFileCallback(DataDeskASTNode *root, char *filename)
{}

DATA_DESK_FUNC void
DataDeskCustomConstantCallback(DataDeskConstant constant_info, char *filename)
{}

DATA_DESK_FUNC void
DataDeskCustomStructCallback(DataDeskStruct struct_info, char *filename)
{
    if(DataDeskStructHasTag(struct_info, "Entity"))
    {
        if(global_entity_count < sizeof(global_entities) / sizeof(global_entities[0]))
        {
            Entity entity = {
                struct_info.name,
                struct_info.name_lowercase_with_underscores,
                struct_info.name_uppercase_with_underscores,
                struct_info.name_upper_camel_case,
                struct_info.name_lower_camel_case,
                struct_info.root,
            };
            global_entities[global_entity_count++] = entity;
        }
    }
    else
    {
        DataDeskFWriteStructAsC(global_entity_file, struct_info);
    }
}

DATA_DESK_FUNC void
DataDeskCustomDeclarationCallback(DataDeskDeclaration declaration_info, char *filename)
{}

DATA_DESK_FUNC void
DataDeskCustomCleanUpCallback(void)
{
    FILE *file = global_entity_file;
    if(file)
    {
        for(int i = 0; i < global_entity_count; ++i)
        {
            Entity *entity = global_entities + i;
            
            DataDeskASTNode *box_component = 0;
            DataDeskASTNode *static_floating_box_component = 0;
            DataDeskASTNode *sprite_component = 0;
            
            for(DataDeskASTNode *member = entity->root->struct_declaration.first_member;
                member;
                member = member->next)
            {
                if(DataDeskDeclarationIsType(member, "BoxComponent"))
                {
                    box_component = member;
                }
                else if(DataDeskDeclarationIsType(member, "StaticFloatingBoxComponent"))
                {
                    static_floating_box_component = member;
                }
                else if(DataDeskDeclarationIsType(member, "SpriteComponent"))
                {
                    sprite_component = member;
                }
            }
            
            // NOTE(rjf): Generate struct definition
            {
                fprintf(file, "typedef struct %sSet\n{\n", entity->name_upper_camel_case);
                
                fprintf(file, "u32 count;\n");
                fprintf(file, "u32 max_count;\n");
                fprintf(file, "u32 *id_to_index_table;\n");
                fprintf(file, "u32 *index_to_id_table;\n");
                fprintf(file, "u32 free_id_count;\n");
                fprintf(file, "u32 *free_id_list;\n");
                
                for(DataDeskASTNode *member = entity->root->struct_declaration.first_member;
                    member;
                    member = member->next)
                {
                    fprintf(file, "%s *%s;\n", member->declaration.type->string, member->string);
                }
                
                fprintf(file, "}\n%sSet;\n\n", entity->name_upper_camel_case);
            }
            
            // NOTE(rjf): Generate initialization function
            {
                fprintf(file, "void %sSetInit(%sSet *set, u32 maximum, MemoryArena *arena)\n{\n",
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case);
                fprintf(file, "set->count = 0;\n");
                fprintf(file, "set->max_count = maximum;\n");
                
                fprintf(file, "set->id_to_index_table = MemoryArenaAllocate(arena, sizeof(u32) * maximum);\n");
                fprintf(file, "set->index_to_id_table = MemoryArenaAllocate(arena, sizeof(u32) * maximum);\n");
                fprintf(file, "set->free_id_count = maximum;\n");
                fprintf(file, "set->free_id_list = MemoryArenaAllocate(arena, sizeof(u32) * maximum);\n\n");
                
                fprintf(file, "for(u32 i = 0; i < maximum; ++i)\n{\n");
                fprintf(file, "set->free_id_list[i] = maximum - i - 1;\n");
                fprintf(file, "}\n\n");
                
                for(DataDeskASTNode *member = entity->root->struct_declaration.first_member;
                    member;
                    member = member->next)
                {
                    fprintf(file, "set->%s = MemoryArenaAllocate(arena, sizeof(set->%s[0]) * maximum);\n",
                            member->string, member->string);
                }
                
                fprintf(file, "}\n\n");
            }
            
            // NOTE(rjf): Generate ID gen function
            {
                fprintf(file, "u32 %sSetGetNewID(%sSet *set)\n{\n",
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case);
                
                fprintf(file, "u32 id = 0;\n");
                fprintf(file, "if(set->free_id_count > 0)\n{\n");
                fprintf(file, "--set->free_id_count;\n");
                fprintf(file, "id = set->free_id_list[set->free_id_count];\n");
                fprintf(file, "}\n");
                fprintf(file, "return id;\n");
                
                fprintf(file, "}\n\n");
            }
            
            // NOTE(rjf): Generate adding function
            {
                fprintf(file, "int %sSetAdd(%sSet *set",
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case);
                
                for(DataDeskASTNode *member = entity->root->struct_declaration.first_member;
                    member;
                    member = member->next)
                {
                    fprintf(file, ", %s %s",
                            member->declaration.type->string,
                            member->string);
                }
                
                fprintf(file, ")\n{\n");
                fprintf(file, "int success = 0;\n");
                fprintf(file, "if(set->count < set->max_count)\n{\n");
                fprintf(file, "success = 1;\n");
                fprintf(file, "u32 index = set->count++;\n");
                fprintf(file, "u32 id = %sSetGetNewID(set);\n", entity->name_upper_camel_case);
                
                fprintf(file, "set->index_to_id_table[index] = id;\n");
                fprintf(file, "set->id_to_index_table[id] = index;\n");
                
                for(DataDeskASTNode *member = entity->root->struct_declaration.first_member;
                    member;
                    member = member->next)
                {
                    fprintf(file, "set->%s[index] = %s;\n",
                            member->string, member->string);
                }
                
                fprintf(file, "}\n");
                fprintf(file, "return success;\n");
                fprintf(file, "}\n\n");
            }
            
            // NOTE(rjf): Generate removal by index function
            {
                fprintf(file, "int %sSetRemoveByIndex(%sSet *set, u32 index)\n{\n",
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case);
                
                fprintf(file, "int success = 0;\n\n");
                fprintf(file, "if(index >= 0 && index < set->count)\n{\n");
                fprintf(file, "success = 1;\n");
                fprintf(file, "if(index != --set->count)\n{\n");
                fprintf(file, "set->free_id_list[set->free_id_count++] = set->index_to_id_table[index];\n");
                fprintf(file, "u32 replacement_index = set->count;\n");
                fprintf(file, "u32 replacement_id = set->index_to_id_table[replacement_index];\n");
                fprintf(file, "set->id_to_index_table[replacement_id] = replacement_index;\n");
                fprintf(file, "set->index_to_id_table[replacement_index] = replacement_id;\n");
                
                for(DataDeskASTNode *member = entity->root->struct_declaration.first_member;
                    member;
                    member = member->next)
                {
                    fprintf(file, "set->%s[index] = set->%s[replacement_index];\n",
                            member->string, member->string);
                }
                
                fprintf(file, "}\n\n");
                fprintf(file, "}\n\n");
                fprintf(file, "return success;\n");
                fprintf(file, "}\n\n");
            }
            
            // NOTE(rjf): Generate removal by ID function
            {
                fprintf(file, "int %sSetRemoveByID(%sSet *set, u32 id)\n{\n",
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case);
                fprintf(file, "return %sSetRemoveByIndex(set, set->id_to_index_table[id]);\n",
                        entity->name_upper_camel_case);
                fprintf(file, "}\n\n");
            }
            
            // NOTE(rjf): Generate update function
            {
                fprintf(file, "void %sSetUpdate(%sSet *set, CollisionMap collision_map)\n{\n", entity->name_upper_camel_case,
                        entity->name_upper_camel_case);
                if(box_component)
                {
                    fprintf(file, "CollideBoxComponentsWithCollisionMap(set->%s, set->count, collision_map);\n",
                            box_component->string);
                    fprintf(file, "UpdateBoxComponents(set->%s, set->count);\n",
                            box_component->string);
                    
                    if(sprite_component)
                    {
                        fprintf(file, "TrackSpriteComponentsToBoxComponents(set->%s, set->%s, set->count);\n",
                                sprite_component->string, box_component->string);
                    }
                }
                else if(static_floating_box_component)
                {
                    fprintf(file, "UpdateStaticFloatingBoxComponents(set->%s, set->count);\n",
                            static_floating_box_component->string);
                    if(sprite_component)
                    {
                        fprintf(file, "TrackSpriteComponentsToStaticFloatingBoxComponents(set->%s, set->%s, set->count);\n",
                                sprite_component->string, static_floating_box_component->string);
                    }
                }
                
                fprintf(file, "}\n\n");
            }
            
            // NOTE(rjf): Generate render function
            {
                fprintf(file, "void %sSetRender(%sSet *set, Camera camera)\n{\n", entity->name_upper_camel_case,
                        entity->name_upper_camel_case);
                
                if(sprite_component)
                {
                    fprintf(file, "RenderSpriteComponents(set->%s, set->count, camera);\n",
                            sprite_component->string);
                }
                
                fprintf(file, "}\n\n");
            }
            
            // NOTE(rjf): Generate moving functions
            {
                fprintf(file, "void %sSetMove%sByIndex(%sSet *set, u32 index, v2 target)\n{\n",
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case);
                
                if(box_component)
                {
                    fprintf(file, "set->%s[index].position = target;\n", box_component->string);
                }
                else if(static_floating_box_component)
                {
                    fprintf(file, "set->%s[index].position = target;\n", static_floating_box_component->string);
                }
                
                fprintf(file, "}\n\n");
                
                fprintf(file, "void %sSetMove%sByID(%sSet *set, u32 id, v2 target)\n{\n",
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case);
                fprintf(file, "%sSetMove%sByIndex(set, set->id_to_index_table[id], target);\n",
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case);
                fprintf(file, "}\n\n");
            }
            
            // NOTE(rjf): Generate bounding box getting functions
            {
                fprintf(file, "v4 %sSetGet%sBoundingBoxByIndex(%sSet *set, u32 index)\n{\n",
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case);
                
                fprintf(file, "v4 result = {0};\n");
                
                if(box_component)
                {
                    fprintf(file,
                            "result.x = set->%s[index].position.x;\n"
                            "result.y = set->%s[index].position.y;\n"
                            "result.width = set->%s[index].size.x;\n"
                            "result.height = set->%s[index].size.y;\n",
                            box_component->string,
                            box_component->string,
                            box_component->string,
                            box_component->string);
                }
                else if(static_floating_box_component)
                {
                    fprintf(file,
                            "result.x = set->%s[index].position.x;\n"
                            "result.y = set->%s[index].position.y;\n"
                            "result.width = set->%s[index].size.x;\n"
                            "result.height = set->%s[index].size.y;\n",
                            static_floating_box_component->string,
                            static_floating_box_component->string,
                            static_floating_box_component->string,
                            static_floating_box_component->string);
                }
                
                fprintf(file, "return result;\n");
                
                fprintf(file, "}\n\n");
                
                fprintf(file, "v4 %sSetGet%sBoundingBoxByID(%sSet *set, u32 id)\n{\n",
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case);
                fprintf(file, "return %sSetGet%sBoundingBoxByIndex(set, set->id_to_index_table[id]);\n",
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case,
                        entity->name_upper_camel_case);
                fprintf(file, "}\n\n");
                
            }
            
        }
        
        fclose(global_entity_file);
    }
}
