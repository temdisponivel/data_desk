typedef struct BoxComponent BoxComponent;
struct BoxComponent
{
v2 position;
v2 velocity;
v2 size;
};

typedef struct StaticFloatingBoxComponent StaticFloatingBoxComponent;
struct StaticFloatingBoxComponent
{
v2 position;
v2 size;
f32 float_sin_pos;
};

typedef struct SpriteComponent SpriteComponent;
struct SpriteComponent
{
v2 position;
Texture *texture;
v2 offset;
v2 size;
f32 angle;
};

typedef struct PlayerSet
{
u32 count;
u32 max_count;
u32 *id_to_index_table;
u32 *index_to_id_table;
u32 free_id_count;
u32 *free_id_list;
BoxComponent *box;
SpriteComponent *sprite;
}
PlayerSet;

void PlayerSetInit(PlayerSet *set, u32 maximum, MemoryArena *arena)
{
set->count = 0;
set->max_count = maximum;
set->id_to_index_table = MemoryArenaAllocate(arena, sizeof(u32) * maximum);
set->index_to_id_table = MemoryArenaAllocate(arena, sizeof(u32) * maximum);
set->free_id_count = maximum;
set->free_id_list = MemoryArenaAllocate(arena, sizeof(u32) * maximum);

for(u32 i = 0; i < maximum; ++i)
{
set->free_id_list[i] = maximum - i - 1;
}

set->box = MemoryArenaAllocate(arena, sizeof(set->box[0]) * maximum);
set->sprite = MemoryArenaAllocate(arena, sizeof(set->sprite[0]) * maximum);
}

u32 PlayerSetGetNewID(PlayerSet *set)
{
u32 id = 0;
if(set->free_id_count > 0)
{
--set->free_id_count;
id = set->free_id_list[set->free_id_count];
}
return id;
}

int PlayerSetAdd(PlayerSet *set, BoxComponent box, SpriteComponent sprite)
{
int success = 0;
if(set->count < set->max_count)
{
success = 1;
u32 index = set->count++;
u32 id = PlayerSetGetNewID(set);
set->index_to_id_table[index] = id;
set->id_to_index_table[id] = index;
set->box[index] = box;
set->sprite[index] = sprite;
}
return success;
}

int PlayerSetRemoveByIndex(PlayerSet *set, u32 index)
{
int success = 0;

if(index >= 0 && index < set->count)
{
success = 1;
if(index != --set->count)
{
set->free_id_list[set->free_id_count++] = set->index_to_id_table[index];
u32 replacement_index = set->count;
u32 replacement_id = set->index_to_id_table[replacement_index];
set->id_to_index_table[replacement_id] = replacement_index;
set->index_to_id_table[replacement_index] = replacement_id;
set->box[index] = set->box[replacement_index];
set->sprite[index] = set->sprite[replacement_index];
}

}

return success;
}

int PlayerSetRemoveByID(PlayerSet *set, u32 id)
{
return PlayerSetRemoveByIndex(set, set->id_to_index_table[id]);
}

void PlayerSetUpdate(PlayerSet *set, CollisionMap collision_map)
{
CollideBoxComponentsWithCollisionMap(set->box, set->count, collision_map);
UpdateBoxComponents(set->box, set->count);
TrackSpriteComponentsToBoxComponents(set->sprite, set->box, set->count);
}

void PlayerSetRender(PlayerSet *set, Camera camera)
{
RenderSpriteComponents(set->sprite, set->count, camera);
}

void PlayerSetMovePlayerByIndex(PlayerSet *set, u32 index, v2 target)
{
set->box[index].position = target;
}

void PlayerSetMovePlayerByID(PlayerSet *set, u32 id, v2 target)
{
PlayerSetMovePlayerByIndex(set, set->id_to_index_table[id], target);
}

v4 PlayerSetGetPlayerBoundingBoxByIndex(PlayerSet *set, u32 index)
{
v4 result = {0};
result.x = set->box[index].position.x;
result.y = set->box[index].position.y;
result.width = set->box[index].size.x;
result.height = set->box[index].size.y;
return result;
}

v4 PlayerSetGetPlayerBoundingBoxByID(PlayerSet *set, u32 id)
{
return PlayerSetGetPlayerBoundingBoxByIndex(set, set->id_to_index_table[id]);
}

typedef struct EnemySet
{
u32 count;
u32 max_count;
u32 *id_to_index_table;
u32 *index_to_id_table;
u32 free_id_count;
u32 *free_id_list;
BoxComponent *box;
SpriteComponent *sprite;
}
EnemySet;

void EnemySetInit(EnemySet *set, u32 maximum, MemoryArena *arena)
{
set->count = 0;
set->max_count = maximum;
set->id_to_index_table = MemoryArenaAllocate(arena, sizeof(u32) * maximum);
set->index_to_id_table = MemoryArenaAllocate(arena, sizeof(u32) * maximum);
set->free_id_count = maximum;
set->free_id_list = MemoryArenaAllocate(arena, sizeof(u32) * maximum);

for(u32 i = 0; i < maximum; ++i)
{
set->free_id_list[i] = maximum - i - 1;
}

set->box = MemoryArenaAllocate(arena, sizeof(set->box[0]) * maximum);
set->sprite = MemoryArenaAllocate(arena, sizeof(set->sprite[0]) * maximum);
}

u32 EnemySetGetNewID(EnemySet *set)
{
u32 id = 0;
if(set->free_id_count > 0)
{
--set->free_id_count;
id = set->free_id_list[set->free_id_count];
}
return id;
}

int EnemySetAdd(EnemySet *set, BoxComponent box, SpriteComponent sprite)
{
int success = 0;
if(set->count < set->max_count)
{
success = 1;
u32 index = set->count++;
u32 id = EnemySetGetNewID(set);
set->index_to_id_table[index] = id;
set->id_to_index_table[id] = index;
set->box[index] = box;
set->sprite[index] = sprite;
}
return success;
}

int EnemySetRemoveByIndex(EnemySet *set, u32 index)
{
int success = 0;

if(index >= 0 && index < set->count)
{
success = 1;
if(index != --set->count)
{
set->free_id_list[set->free_id_count++] = set->index_to_id_table[index];
u32 replacement_index = set->count;
u32 replacement_id = set->index_to_id_table[replacement_index];
set->id_to_index_table[replacement_id] = replacement_index;
set->index_to_id_table[replacement_index] = replacement_id;
set->box[index] = set->box[replacement_index];
set->sprite[index] = set->sprite[replacement_index];
}

}

return success;
}

int EnemySetRemoveByID(EnemySet *set, u32 id)
{
return EnemySetRemoveByIndex(set, set->id_to_index_table[id]);
}

void EnemySetUpdate(EnemySet *set, CollisionMap collision_map)
{
CollideBoxComponentsWithCollisionMap(set->box, set->count, collision_map);
UpdateBoxComponents(set->box, set->count);
TrackSpriteComponentsToBoxComponents(set->sprite, set->box, set->count);
}

void EnemySetRender(EnemySet *set, Camera camera)
{
RenderSpriteComponents(set->sprite, set->count, camera);
}

void EnemySetMoveEnemyByIndex(EnemySet *set, u32 index, v2 target)
{
set->box[index].position = target;
}

void EnemySetMoveEnemyByID(EnemySet *set, u32 id, v2 target)
{
EnemySetMoveEnemyByIndex(set, set->id_to_index_table[id], target);
}

v4 EnemySetGetEnemyBoundingBoxByIndex(EnemySet *set, u32 index)
{
v4 result = {0};
result.x = set->box[index].position.x;
result.y = set->box[index].position.y;
result.width = set->box[index].size.x;
result.height = set->box[index].size.y;
return result;
}

v4 EnemySetGetEnemyBoundingBoxByID(EnemySet *set, u32 id)
{
return EnemySetGetEnemyBoundingBoxByIndex(set, set->id_to_index_table[id]);
}

typedef struct PowerUpSet
{
u32 count;
u32 max_count;
u32 *id_to_index_table;
u32 *index_to_id_table;
u32 free_id_count;
u32 *free_id_list;
StaticFloatingBoxComponent *box;
SpriteComponent *sprite;
}
PowerUpSet;

void PowerUpSetInit(PowerUpSet *set, u32 maximum, MemoryArena *arena)
{
set->count = 0;
set->max_count = maximum;
set->id_to_index_table = MemoryArenaAllocate(arena, sizeof(u32) * maximum);
set->index_to_id_table = MemoryArenaAllocate(arena, sizeof(u32) * maximum);
set->free_id_count = maximum;
set->free_id_list = MemoryArenaAllocate(arena, sizeof(u32) * maximum);

for(u32 i = 0; i < maximum; ++i)
{
set->free_id_list[i] = maximum - i - 1;
}

set->box = MemoryArenaAllocate(arena, sizeof(set->box[0]) * maximum);
set->sprite = MemoryArenaAllocate(arena, sizeof(set->sprite[0]) * maximum);
}

u32 PowerUpSetGetNewID(PowerUpSet *set)
{
u32 id = 0;
if(set->free_id_count > 0)
{
--set->free_id_count;
id = set->free_id_list[set->free_id_count];
}
return id;
}

int PowerUpSetAdd(PowerUpSet *set, StaticFloatingBoxComponent box, SpriteComponent sprite)
{
int success = 0;
if(set->count < set->max_count)
{
success = 1;
u32 index = set->count++;
u32 id = PowerUpSetGetNewID(set);
set->index_to_id_table[index] = id;
set->id_to_index_table[id] = index;
set->box[index] = box;
set->sprite[index] = sprite;
}
return success;
}

int PowerUpSetRemoveByIndex(PowerUpSet *set, u32 index)
{
int success = 0;

if(index >= 0 && index < set->count)
{
success = 1;
if(index != --set->count)
{
set->free_id_list[set->free_id_count++] = set->index_to_id_table[index];
u32 replacement_index = set->count;
u32 replacement_id = set->index_to_id_table[replacement_index];
set->id_to_index_table[replacement_id] = replacement_index;
set->index_to_id_table[replacement_index] = replacement_id;
set->box[index] = set->box[replacement_index];
set->sprite[index] = set->sprite[replacement_index];
}

}

return success;
}

int PowerUpSetRemoveByID(PowerUpSet *set, u32 id)
{
return PowerUpSetRemoveByIndex(set, set->id_to_index_table[id]);
}

void PowerUpSetUpdate(PowerUpSet *set, CollisionMap collision_map)
{
UpdateStaticFloatingBoxComponents(set->box, set->count);
TrackSpriteComponentsToStaticFloatingBoxComponents(set->sprite, set->box, set->count);
}

void PowerUpSetRender(PowerUpSet *set, Camera camera)
{
RenderSpriteComponents(set->sprite, set->count, camera);
}

void PowerUpSetMovePowerUpByIndex(PowerUpSet *set, u32 index, v2 target)
{
set->box[index].position = target;
}

void PowerUpSetMovePowerUpByID(PowerUpSet *set, u32 id, v2 target)
{
PowerUpSetMovePowerUpByIndex(set, set->id_to_index_table[id], target);
}

v4 PowerUpSetGetPowerUpBoundingBoxByIndex(PowerUpSet *set, u32 index)
{
v4 result = {0};
result.x = set->box[index].position.x;
result.y = set->box[index].position.y;
result.width = set->box[index].size.x;
result.height = set->box[index].size.y;
return result;
}

v4 PowerUpSetGetPowerUpBoundingBoxByID(PowerUpSet *set, u32 id)
{
return PowerUpSetGetPowerUpBoundingBoxByIndex(set, set->id_to_index_table[id]);
}

typedef struct CollectibleSet
{
u32 count;
u32 max_count;
u32 *id_to_index_table;
u32 *index_to_id_table;
u32 free_id_count;
u32 *free_id_list;
StaticFloatingBoxComponent *box;
SpriteComponent *sprite;
}
CollectibleSet;

void CollectibleSetInit(CollectibleSet *set, u32 maximum, MemoryArena *arena)
{
set->count = 0;
set->max_count = maximum;
set->id_to_index_table = MemoryArenaAllocate(arena, sizeof(u32) * maximum);
set->index_to_id_table = MemoryArenaAllocate(arena, sizeof(u32) * maximum);
set->free_id_count = maximum;
set->free_id_list = MemoryArenaAllocate(arena, sizeof(u32) * maximum);

for(u32 i = 0; i < maximum; ++i)
{
set->free_id_list[i] = maximum - i - 1;
}

set->box = MemoryArenaAllocate(arena, sizeof(set->box[0]) * maximum);
set->sprite = MemoryArenaAllocate(arena, sizeof(set->sprite[0]) * maximum);
}

u32 CollectibleSetGetNewID(CollectibleSet *set)
{
u32 id = 0;
if(set->free_id_count > 0)
{
--set->free_id_count;
id = set->free_id_list[set->free_id_count];
}
return id;
}

int CollectibleSetAdd(CollectibleSet *set, StaticFloatingBoxComponent box, SpriteComponent sprite)
{
int success = 0;
if(set->count < set->max_count)
{
success = 1;
u32 index = set->count++;
u32 id = CollectibleSetGetNewID(set);
set->index_to_id_table[index] = id;
set->id_to_index_table[id] = index;
set->box[index] = box;
set->sprite[index] = sprite;
}
return success;
}

int CollectibleSetRemoveByIndex(CollectibleSet *set, u32 index)
{
int success = 0;

if(index >= 0 && index < set->count)
{
success = 1;
if(index != --set->count)
{
set->free_id_list[set->free_id_count++] = set->index_to_id_table[index];
u32 replacement_index = set->count;
u32 replacement_id = set->index_to_id_table[replacement_index];
set->id_to_index_table[replacement_id] = replacement_index;
set->index_to_id_table[replacement_index] = replacement_id;
set->box[index] = set->box[replacement_index];
set->sprite[index] = set->sprite[replacement_index];
}

}

return success;
}

int CollectibleSetRemoveByID(CollectibleSet *set, u32 id)
{
return CollectibleSetRemoveByIndex(set, set->id_to_index_table[id]);
}

void CollectibleSetUpdate(CollectibleSet *set, CollisionMap collision_map)
{
UpdateStaticFloatingBoxComponents(set->box, set->count);
TrackSpriteComponentsToStaticFloatingBoxComponents(set->sprite, set->box, set->count);
}

void CollectibleSetRender(CollectibleSet *set, Camera camera)
{
RenderSpriteComponents(set->sprite, set->count, camera);
}

void CollectibleSetMoveCollectibleByIndex(CollectibleSet *set, u32 index, v2 target)
{
set->box[index].position = target;
}

void CollectibleSetMoveCollectibleByID(CollectibleSet *set, u32 id, v2 target)
{
CollectibleSetMoveCollectibleByIndex(set, set->id_to_index_table[id], target);
}

v4 CollectibleSetGetCollectibleBoundingBoxByIndex(CollectibleSet *set, u32 index)
{
v4 result = {0};
result.x = set->box[index].position.x;
result.y = set->box[index].position.y;
result.width = set->box[index].size.x;
result.height = set->box[index].size.y;
return result;
}

v4 CollectibleSetGetCollectibleBoundingBoxByID(CollectibleSet *set, u32 id)
{
return CollectibleSetGetCollectibleBoundingBoxByIndex(set, set->id_to_index_table[id]);
}

