#include <Collisions.h>
#include <Model.h>

bool checkOverlap(const AABB& playerAABB, const AABB& modelAABB) {
    return (playerAABB.min.x <= modelAABB.max.x && playerAABB.max.x >= modelAABB.min.x) &&
           (playerAABB.min.y <= modelAABB.max.y && playerAABB.max.y >= modelAABB.min.y) &&
           (playerAABB.min.z <= modelAABB.max.z && playerAABB.max.z >= modelAABB.min.z);
}


// void resolveCollision(
//     glm::vec3& pos, 
//     glm::vec3& vel, 
//     float r, 
//     const AABB& box, 
//     float restitution = 0.8f
// ) {
//     // Find closest point on the AABB to the particle center
//     glm::vec3 closest = glm::clamp(pos, box.min, box.max);
//     glm::vec3 delta = pos - closest;
//     float distSq = glm::dot(delta, delta);

//     if (distSq < r * r && distSq > 1e-6f) {
//         float dist = std::sqrt(distSq);
//         glm::vec3 normal = delta / dist;

//         // Push particle out of the boundary
//         pos = closest + normal * r;

//         // Reflect velocity along the collision normal
//         float vn = glm::dot(vel, normal);
//         if (vn < 0.0f) {
//             vel -= (1.0f + restitution) * vn * normal;
//         }
//     }
// }
