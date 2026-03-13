# AIVehicle::UpdateSimplePhysics - Implementation Guide

## Quick Reference

| Item | Value |
|------|-------|
| **File** | `src/Speed/Indep/Src/AI/Common/AIVehicle.cpp` |
| **Header** | `src/Speed/Indep/Src/AI/AIVehicle.h:318` |
| **Call Site** | Line 614 in Update() |
| **Binary Address** | 0x8001F9A4 (GOWE69), 0x11b9a8 (SLES) |
| **Size** | 2000 bytes, ~500 instructions |
| **Status** | ⚠️ MISSING - Needs implementation |
| **Mangled** | `UpdateSimplePhysics__9AIVehiclef` |

## Function Prototype

```cpp
void AIVehicle::UpdateSimplePhysics(float dT);
```

Called from `AIVehicle::Update()` at line 614:

```cpp
void AIVehicle::Update(float dT) {
    // ... angle calculations ...
    UpdateSimplePhysics(dT);  // <- HERE
}
```

## Local Variables (from DWARF)

### Registers
- **r31** - `this` pointer (AIVehicle*)
- **f28** - `dT` parameter (float)
- **r25** - `isimable` (ISimable*)
- **r22** - `ivehicle` (IVehicle*)
- **r26** - `irigidbody` (IRigidBody*)
- **r23** - `position` (UMath::Vector3&)
- **r28** - `wpos` (WWorldPos&)
- **r27** - `up_valid` (bool)
- **f31** - `rideheight` (float)
- **f30** - `driveSpeed` (float)
- **f29** - `skill` (float)

### Stack Variables (relative to SP)
- **SP+0x8**  - `newPosition` (UMath::Vector3)
- **SP+0x18** - `vehicleMat` (UMath::Matrix4)
- **SP+0x58** - `destPos` (UMath::Vector3)
- **SP+0x68** - `dirVector` (UMath::Vector3)
- **SP+0x78** - `newUpVector` (UMath::Vector4)
- **SP+0x88** - `dimension` (UMath::Vector3)
- **SP+0x98** - `linearVelocity` (UMath::Vector3)

Stack frame size: 0x130 bytes

## Algorithm Flow

### Phase 1: Validation
```
if (!IsSimplePhysicsActive()) {
    return;  // Early exit
}
```

### Phase 2: Get Interfaces
```cpp
ISimable* simable = GetSimable();  // r25
IVehicle* vehicle = simable->GetVehicle();  // r22
IRigidBody* rb = simable->GetRigidBody();  // r26

// Get vehicle position
Vector3 position = rb->GetPosition();  // r23
```

### Phase 3: Terrain Interaction
```cpp
// Find closest collision point on terrain
WWorldPos terrain_pos;
FindClosestFace(rb->GetPosition(), terrain_pos);

// Get surface normal
Vector3 surface_normal;
terrain_pos.UNormal(&surface_normal);

// Calculate displacement
Vector3 displacement = terrain_pos - position;
float displacement_len = Normalize(displacement);  // dirVector
```

### Phase 4: Speed Calculation
```cpp
float current_height = terrain_pos.y;
float vehicle_height = position.y;
float height_diff = current_height - vehicle_height;

// Get player skill (0.0 to 1.0)
float skill = GetSkill();  // f29

// Interpolate between min/max acceleration
float accel = Lerp(
    Tweak_OffWorldAccel._0_4_,
    Tweak_OffWorldAccel._4_4_,
    skill
);

// Accumulate vertical speed
vertical_speed += accel * dT * (1.0 - skill) + skill * base_accel;

// Clamp to max
vertical_speed = Min(vertical_speed, max_speed);

// Calculate drive speed
float drive_speed = Lerp(
    Tweak_OffWorldSpeed._0_4_,
    Tweak_OffWorldSpeed._4_4_,
    skill
);

// Apply gravity modifier if conditions met
if (IsUpsideDown()) {
    drive_speed *= -0.5f;  // Reverse/slow when upside down
}
```

### Phase 5: Coordinate Frame Construction

#### Right Vector
```cpp
// Cross product of surface normal and forward direction
Vector3 right_vector = Cross(surface_normal, dirVector);
Normalize(right_vector);
```

#### Up Vector
```cpp
// Create orthogonal up vector
Vector3 up_vector;
if (IsValidSurfaceNormal(surface_normal)) {
    // Cross product of forward and right
    up_vector = Cross(dirVector, right_vector);
    Normalize(up_vector);
    up_valid = true;
} else {
    // Fallback to default up
    up_vector = Vector3(0, 1, 0);
    up_valid = false;
}
```

### Phase 6: Suspension Height
```cpp
// Get suspension system
ISuspension* suspension = GetSuspension();

float max_ride_height = 0.0f;
for (int i = 0; i < 4; i++) {
    float ride_height = suspension->GetRideHeight(i);
    max_ride_height = Max(max_ride_height, ride_height);
}

// Clamp to maximum
max_ride_height = Min(max_ride_height, MAX_RIDEHEIGHT_LIMIT);
```

### Phase 7: Position Update
```cpp
// New position is terrain position + suspension height
Vector3 new_position = terrain_pos;
new_position.y += max_ride_height;

// Set physics state
rb->SetPosition(new_position);
rb->SetForwardVector(dirVector);
rb->SetUpVector(up_vector);
```

### Phase 8: Velocity Update
```cpp
// Linear velocity along displacement direction
Vector3 linear_velocity = dirVector * (drive_speed * dT);
rb->SetLinearVelocity(linear_velocity);

// No rotation in simple physics mode
rb->SetAngularVelocity(Vector3::Zero());
```

## Key Dependencies

### Methods Called
- `IsSimplePhysicsActive()` - Conditionally execute
- `GetSimable()` - Get ISimable interface
- `GetVehicle()` - Get IVehicle from simable
- `GetRigidBody()` - Get IRigidBody from simable
- `GetSuspension()` - Get ISuspension interface
- `FindClosestFace()` - Terrain intersection
- `WWorldPos::UNormal()` - Get surface normal
- `WWorldPos::HeightAtPoint()` - Query height
- `UMath::Cross()` - Vector cross product
- `UMath::Unit()` - Vector normalization
- `UMath::Lerp()` - Skill-based interpolation

### Physics Setters
- `IRigidBody::SetPosition(Vector3)`
- `IRigidBody::SetForwardVector(Vector3)`
- `IRigidBody::SetUpVector(Vector3)`
- `IRigidBody::SetLinearVelocity(Vector3)`
- `IRigidBody::SetAngularVelocity(Vector3)`

## Tweakable Parameters

```cpp
// Located at 0x803c9fa4-0x803c9fb8 (estimated)
struct {
    float Tweak_OffWorldAccel[2];      // Min/Max acceleration
    float Tweak_OffWorldSpeed[2];      // Min/Max speed
    float gravity_param;                // ~30.0
    float gravity_modifier;             // -0.5
    float max_rideheight;               // Vehicle-specific
    float normal_threshold;             // Validation tolerance
} tweaks;
```

## Implementation Considerations

1. **Early Exit**: Check `IsSimplePhysicsActive()` first to avoid unnecessary computation
2. **Vector Normalization**: All direction vectors should be normalized before use
3. **Suspension System**: Must account for all 4 wheels' ride heights
4. **Skill Interpolation**: Use Lerp for smooth acceleration/speed transitions based on AI skill
5. **Coordinate Frames**: Ensure orthonormal basis (all vectors perpendicular and normalized)
6. **Physics Constraints**: Angular velocity must be ZERO (no spinning)
7. **Upside-Down Handling**: Apply modifier when vehicle is inverted
8. **Terrain Validation**: Check surface normal validity before using for up vector

## Pseudo-Code Template

```cpp
void AIVehicle::UpdateSimplePhysics(float dT) {
    // 1. Early exit
    if (!IsSimplePhysicsActive()) return;
    
    // 2. Get interfaces
    ISimable* simable = GetSimable();
    if (!simable) return;
    IVehicle* vehicle = GetVehicle();
    IRigidBody* rb = simable->GetRigidBody();
    if (!rb) return;
    
    // 3. Get positions
    Vector3 vehicle_pos = rb->GetPosition();
    WWorldPos terrain_pos;
    FindClosestFace(vehicle_pos, terrain_pos);
    
    // 4. Calculate direction
    Vector3 dir = terrain_pos - vehicle_pos;
    float dist = Normalize(dir);
    
    // 5. Get terrain normal
    Vector3 normal;
    terrain_pos.UNormal(&normal);
    
    // 6. Calculate speed
    float skill = GetSkill();
    float height_diff = terrain_pos.y - vehicle_pos.y;
    float speed = InterpolateSpeed(skill, height_diff, dT);
    
    // 7. Build frame
    Vector3 right = Cross(normal, dir);
    Normalize(right);
    Vector3 up = Cross(dir, right);
    Normalize(up);
    
    // 8. Get suspension height
    float ride_height = GetMaxRideHeight();
    
    // 9. Update physics
    Vector3 new_pos = terrain_pos;
    new_pos.y += ride_height;
    rb->SetPosition(new_pos);
    rb->SetForwardVector(dir);
    rb->SetUpVector(up);
    rb->SetLinearVelocity(dir * speed);
    rb->SetAngularVelocity(Vector3::Zero());
}
```

## Testing Checklist

- [ ] Function correctly returns early when simple physics is disabled
- [ ] Terrain intersection detection works in various terrain types
- [ ] Surface normal calculation produces valid orthonormal vectors
- [ ] Skill-based speed interpolation varies smoothly (0.0 to 1.0)
- [ ] Suspension height offsets position correctly
- [ ] Vehicle follows terrain contours realistically
- [ ] No rotation (angular velocity stays zero)
- [ ] Upside-down gravity modifier applies correctly
- [ ] Frame rate independent (deltaTime scaling works)

---

**Full Decompilation Reference**: See `AIVEHICLE_DECOMPILATION_FULL.txt` for complete assembly and Ghidra output.
