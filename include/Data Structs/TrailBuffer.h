#ifndef TRAILBUFFER_H
#define TRAILBUFFER_H

#include <vector>

#include <glm/glm.hpp>

class TrailBuffer {
    public:
        std::vector<glm::vec3> trail_points;
        int trailCount;

        float trailDuration;

        TrailBuffer(float pollTime, float trailDuration) {
            trailCount = trailDuration / pollTime;
        }

        void addTrailPoint(glm::vec3 p) {
            trail_points.push_back(p);

            if (trail_points.size() > trailCount) {
                trail_points.erase(trail_points.begin());
            }
        }

        int size() const {
            return trail_points.size();
        }

        const glm::vec3* data() const {
            return trail_points.data();
        }

        bool empty() const {
            return trail_points.empty();
        }

};



#endif //TRAILBUFFER_H
