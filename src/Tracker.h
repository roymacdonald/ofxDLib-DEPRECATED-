//
//  Tracker.h
//  this code is borrowed from the Tracker class of ofxCv
//  and modified to be useable with OF math and rectangle classes
//
//  Created by Andreas on 2016.04.20
//
//

#pragma once

#include <utility>
#include <map>
#include "ofRectangle.h"
#include "ofVec2f.h"
#include "ofVec3f.h"
#include "ofMath.h"

namespace ofxDLib {
    float trackingDistance(const ofRectangle& a, const ofRectangle& b) {
        ofVec3f centerA = a.getCenter();
        ofVec3f centerB = b.getCenter();
        return centerA.distance(centerB);
    }
    
    float trackingDistance(const ofVec2f& a, const ofVec2f& b) {
        return a.distance(b);
    }
    
    template <class T>
    class TrackedObject {
    protected:
        unsigned int lastSeen, label, age;
        int index;
    public:
        T object;
        
        TrackedObject(const T& object, unsigned int label, int index)
        :lastSeen(0)
        ,label(label)
        ,age(0)
        ,index(index)
        ,object(object){
        }
        TrackedObject(const T& object, const TrackedObject<T>& previous, int index)
        :lastSeen(0)
        ,label(previous.label)
        ,age(previous.age)
        ,index(index)
        ,object(object){
        }
        TrackedObject(const TrackedObject<T>& old)
        :lastSeen(old.lastSeen)
        ,label(old.label)
        ,age(old.age)
        ,index(-1)
        ,object(old.object){
        }
        void timeStep(bool visible) {
            age++;
            if(!visible) {
                lastSeen++;
            }
        }
        unsigned int getLastSeen() const {
//            ofLog()<<"lastSeen "<<lastSeen;
            return lastSeen;
        }
        unsigned long getAge() const {
            return age;
        }
        unsigned int getLabel() const {
            return label;
        }
        int getIndex() const {
            return index;
        }
    };
    
    struct bySecond {
        template <class First, class Second>
        bool operator()(std::pair<First, Second> const &a, std::pair<First, Second> const &b) {
            return a.second < b.second;
        }
    };
    
    template <class T>
    class Tracker {
    protected:
        vector<TrackedObject<T> > previous, current;
        vector<unsigned int> currentLabels, previousLabels, newLabels, deadLabels;
        std::map<unsigned int, TrackedObject<T>*> previousLabelMap, currentLabelMap;
        
        unsigned int persistence, curLabel;
        float maximumDistance;
        unsigned int getNewLabel() {
            return curLabel++;
        }
        
    public:
        Tracker<T>()
        :persistence(15)
        ,curLabel(0)
        ,maximumDistance(64) {
        }
        virtual ~Tracker(){};
        void setPersistence(unsigned int persistence);
        void setMaximumDistance(float maximumDistance);
        virtual const std::vector<unsigned int>& track(const std::vector<T>& objects);
        
        // organized in the order received by track()
        const std::vector<unsigned int>& getCurrentLabels() const;
        const std::vector<unsigned int>& getPreviousLabels() const;
        const std::vector<unsigned int>& getNewLabels() const;
        const std::vector<unsigned int>& getDeadLabels() const;
        unsigned int getLabelFromIndex(unsigned int i) const;
        
        // organized by label
        int getIndexFromLabel(unsigned int label) const;
        const T& getPrevious(unsigned int label) const;
        const T& getCurrent(unsigned int label) const;
        bool existsCurrent(unsigned int label) const;
        bool existsPrevious(unsigned int label) const;
        int getAge(unsigned int label) const;
        int getLastSeen(unsigned int label) const;
    };
    
    template <class T>
    void Tracker<T>::setPersistence(unsigned int persistence) {
        this->persistence = persistence;
    }
    
    template <class T>
    void Tracker<T>::setMaximumDistance(float maximumDistance) {
        this->maximumDistance = maximumDistance;
    }
    
    template <class T>
    const std::vector<unsigned int>& Tracker<T>::track(const std::vector<T>& objects) {
        previous = current;
        int n = objects.size();
        int m = previous.size();
        
        // build NxM distance matrix
        typedef std::pair<int, int> MatchPair;
        typedef std::pair<MatchPair, float> MatchDistancePair;
        std::vector<MatchDistancePair> all;
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < m; j++) {
                float curDistance = trackingDistance(objects[i], previous[j].object);
                if(curDistance < maximumDistance) {
                    all.push_back(MatchDistancePair(MatchPair(i, j), curDistance));
                }
            }
        }
        
        // sort all possible matches by distance
        sort(all.begin(), all.end(), bySecond());
        
        previousLabels = currentLabels;
        currentLabels.clear();
        currentLabels.resize(n);
        current.clear();
        std::vector<bool> matchedObjects(n, false);
        std::vector<bool> matchedPrevious(m, false);
        // walk through matches in order
        for(int k = 0; k < (int)all.size(); k++) {
            MatchPair& match = all[k].first;
            int i = match.first;
            int j = match.second;
            // only use match if both objects are unmatched, lastSeen is set to 0
            if(!matchedObjects[i] && !matchedPrevious[j]) {
                matchedObjects[i] = true;
                matchedPrevious[j] = true;
                int index = current.size();
                current.push_back(TrackedObject<T>(objects[i], previous[j], index));
                current.back().timeStep(true);
                currentLabels[i] = current.back().getLabel();
            }
        }
        
        // create new labels for new unmatched objects, lastSeen is set to 0
        newLabels.clear();
        for(int i = 0; i < n; i++) {
            if(!matchedObjects[i]) {
                int curLabel = getNewLabel();
                int index = current.size();
                current.push_back(TrackedObject<T>(objects[i], curLabel, index));
                current.back().timeStep(true);
                currentLabels[i] = curLabel;
                newLabels.push_back(curLabel);
            }
        }
        
        // copy old unmatched objects if young enough, lastSeen is increased
        deadLabels.clear();
        for(int j = 0; j < m; j++) {
            if(!matchedPrevious[j]) {
                if(previous[j].getLastSeen() < persistence) {
                    current.push_back(previous[j]);
                    current.back().timeStep(false);
                }
                deadLabels.push_back(previous[j].getLabel());
            }
        }
        
        // build label maps
        currentLabelMap.clear();
        for(int i = 0; i < (int)current.size(); i++) {
            unsigned int label = current[i].getLabel();
            currentLabelMap[label] = &(current[i]);
        }
        previousLabelMap.clear();
        for(int i = 0; i < (int)previous.size(); i++) {
            unsigned int label = previous[i].getLabel();
            previousLabelMap[label] = &(previous[i]);
        }
        
        return currentLabels;
    }
    
    template <class T>
    const std::vector<unsigned int>& Tracker<T>::getCurrentLabels() const {
        return currentLabels;
    }
    
    template <class T>
    const std::vector<unsigned int>& Tracker<T>::getPreviousLabels() const {
        return previousLabels;
    }
    
    template <class T>
    const std::vector<unsigned int>& Tracker<T>::getNewLabels() const {
        return newLabels;
    }
    
    template <class T>
    const vector<unsigned int>& Tracker<T>::getDeadLabels() const {
        return deadLabels;
    }
    
    template <class T>
    unsigned int Tracker<T>::getLabelFromIndex(unsigned int i) const {
        return currentLabels[i];
    }
    
    template <class T>
    int Tracker<T>::getIndexFromLabel(unsigned int label) const {
        return currentLabelMap.find(label)->second->getIndex();
    }
    
    template <class T>
    const T& Tracker<T>::getPrevious(unsigned int label) const {
        return previousLabelMap.find(label)->second->object;
    }
    
    template <class T>
    const T& Tracker<T>::getCurrent(unsigned int label) const {
        return currentLabelMap.find(label)->second->object;
    }
    
    template <class T>
    bool Tracker<T>::existsCurrent(unsigned int label) const {
        return currentLabelMap.count(label) > 0;
    }
    
    template <class T>
    bool Tracker<T>::existsPrevious(unsigned int label) const {
        return previousLabelMap.count(label) > 0;
    }
    
    template <class T>
    int Tracker<T>::getAge(unsigned int label) const{
        return currentLabelMap.find(label)->second->getAge();
    }
    
    template <class T>
    int Tracker<T>::getLastSeen(unsigned int label) const{
        return currentLabelMap.find(label)->second->getLastSeen();
    }
    
    class RectTracker : public Tracker<ofRectangle> {
    protected:
        float smoothingRate;
        std::map<unsigned int, ofRectangle> smoothed;
    public:
        RectTracker()
        :smoothingRate(.5) {
        }
        void setSmoothingRate(float smoothingRate) {
            this->smoothingRate = smoothingRate;
        }
        float getSmoothingRate() const {
            return smoothingRate;
        }
        const std::vector<unsigned int>& track(const std::vector<ofRectangle>& objects) {
            const std::vector<unsigned int>& labels = Tracker<ofRectangle>::track(objects);
            // add new objects, update old objects
            for(int i = 0; i < labels.size(); i++) {
                unsigned int label = labels[i];
                const ofRectangle& cur = getCurrent(label);
                if(smoothed.count(label) > 0) {
                    ofRectangle& smooth = smoothed[label];
                    smooth.x = ofLerp(smooth.x, cur.x, smoothingRate);
                    smooth.y = ofLerp(smooth.y, cur.y, smoothingRate);
                    smooth.width = ofLerp(smooth.width, cur.width, smoothingRate);
                    smooth.height = ofLerp(smooth.height, cur.height, smoothingRate);
                } else {
                    smoothed[label] = cur;
                }
            }
            std::map<unsigned int, ofRectangle>::iterator smoothedItr = smoothed.begin();
            while(smoothedItr != smoothed.end()) {
                unsigned int label = smoothedItr->first;
                if(!existsCurrent(label)) {
                    smoothed.erase(smoothedItr++);
                } else {
                    ++smoothedItr;
                }
            }
            return labels;
        }
        const ofRectangle& getSmoothed(unsigned int label) const {
            return smoothed.find(label)->second;
        }
       ofVec2f getVelocity(unsigned int i) const {
            unsigned int label = getLabelFromIndex(i);
            if(existsPrevious(label)) {
                const ofRectangle& previous = getPrevious(label);
                const ofRectangle& current = getCurrent(label);
                ofVec2f previousPosition(previous.x + previous.width / 2, previous.y + previous.height / 2);
                ofVec2f currentPosition(current.x + current.width / 2, current.y + current.height / 2);
                return currentPosition - previousPosition;
            } else {
                return ofVec2f(0, 0);
            }
        }
    };
    
    typedef Tracker<ofVec2f> PointTracker;
    
    template <class T>
    class Follower {
    protected:
        bool dead;
        unsigned int label;
    public:
        Follower()
        :dead(false)
        ,label(0) {}
        
        virtual ~Follower(){};
        virtual void setup(const T& track) {}
        virtual void update(const T& track) {}
        virtual void kill() {
            dead = true;
        }
        
        void setLabel(unsigned int label) {
            this->label = label;
        }
        unsigned int getLabel() const {
            return label;
        }
        bool getDead() const {
            return dead;
        }
    };
    
    typedef Follower<ofRectangle> RectFollower;
    typedef Follower<ofVec2f> PointFollower;
    
    template <class T, class F>
    class TrackerFollower : public Tracker<T> {
    protected:
        std::vector<unsigned int> labels;
        std::vector<F> followers;
    public:
        const std::vector<unsigned int>& track(const std::vector<T>& objects) {
            Tracker<T>::track(objects);
            // kill missing, update old
            for(int i = 0; i < labels.size(); i++) {
                unsigned int curLabel = labels[i];
                F& curFollower = followers[i];
                if(!Tracker<T>::existsCurrent(curLabel)) {
                    curFollower.kill();
                } else {
                    curFollower.update(Tracker<T>::getCurrent(curLabel));
                }
            }
            // add new
            for(int i = 0; i < Tracker<T>::newLabels.size(); i++) {
                unsigned int curLabel = Tracker<T>::newLabels[i];
                labels.push_back(curLabel);
                followers.push_back(F());
                followers.back().setup(Tracker<T>::getCurrent(curLabel));
                followers.back().setLabel(curLabel);
            }
            // remove dead
            for(int i = labels.size() - 1; i >= 0; i--) {
                if(followers[i].getDead()) {
                    followers.erase(followers.begin() + i);
                    labels.erase(labels.begin() + i);
                }
            }
            return labels;
        }
        std::vector<F>& getFollowers() {
            return followers;
        }
    };
    
    template <class F> class RectTrackerFollower : public TrackerFollower<ofRectangle, F> {};
    template <class F> class PointTrackerFollower : public TrackerFollower<ofVec2f, F> {};
}
