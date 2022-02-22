//
//  MPRoomModel.h
//  Malperdy
//
//  This class encapsulates all of the information for a single room, including
//  its geometry for both drawing and physics. Locations of everything within a
//  a room are stored relative to the room's origin, which is the lower left corner.
// 
//  Room is a subclass of SceneNode, and so all of SceneNode's methods can be used
//  with it. This allows individual rooms and their contents to be scaled properly
//  when zooming in and out.
// 
//  In theory, all the rooms in a level can be made children of a SceneNode for the
//  whole level grid. Transforming the grid should then allow for easy transformation
//  of all the room, and calling render() on the level grid SceneNode should draw all
//  of the rooms accordingly.
//
//  Created by Barry Wang on 2/21/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#ifndef MPRoomModel_h
#define MPRoomModel_h

#include <cugl/cugl.h>
#include <stdlib.h>
#include <vector>

/** The default width of a room in pixels */
#define DEFAULT_ROOM_WIDTH 720
/** The default height of a room in pixels */
#define DEFAULT_ROOM_HEIGHT 480

class RoomModel : public cugl::scene2::SceneNode {
private:
    // GEOMETRY
    /** Vector of Poly2s forming the visuals for the room's geometry */
    shared_ptr<vector<Poly2>> _geometry;
    /** Vector of physics objects forming the room's geometry */
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> _physicsGeometry;

    /**
     * Rebuilds the geometry.
     *
     * This method should recreate all the polygons for any geometry in the room.
     * It should also recreate all physics objects.
     */
    void buildGeometry();

public:
#pragma mark Constructors
    /**
     * Creates a new, empty room.
     */
    RoomModel() {};

    /**
     * Initializes an empty room at the world origin.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @return  true if the room is initialized properly, false otherwise.
     */
    bool init() { init(0, 0); }

    /**
     * Initializes an empty room at the given location.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @param pos   The origin of the room in parent space
     * @return      true if the room is initialized properly, false otherwise.
     */
    bool init(const Vec2 pos) { init(pos.x, pos.y); }

    /**
     * Initializes an empty room at the given location.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @param x The x-coordinate of the room in parent space
     * @param y The y-coordinate of the room in parent space
     * @return  true if the room is initialized properly, false otherwise.
     */
    bool init(float x, float y) { init(x, y, nullptr); }

    /**
     * Initializes a room with the given geometry at the world origin.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @param geometry  Shared pointer to the vector of polygons containing the room's geometry
     * @return          true if the room is initialized properly, false otherwise.
     */
    bool init(shared_ptr<vector<Poly2>> geometry) { init(0, 0, geometry); }

    /**
     * Initializes a room with the given geometry at the given location.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @param pos       The origin of the room in parent space
     * @param geometry  Shared pointer to the vector of polygons containing the room's geometry
     * @return          true if the room is initialized properly, false otherwise.
     */
    bool init(Vec2 pos, shared_ptr<vector<Poly2>> geometry) { init(pos.x, pos.y, geometry); }

    /**
     * Initializes a room with the given geometry at the given location.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @param x         The x-coordinate of the room in parent space
     * @param y         The y-coordinate of the room in parent space
     * @param geometry  Shared pointer to the vector of polygons containing the room's geometry
     * @return          true if the room is initialized properly, false otherwise.
     */
    bool init(float x, float y, shared_ptr<vector<Poly2>> geometry);

#pragma mark Destructors
    /**
     * Destroys this room, releasing all resources.
     */
    virtual ~RoomModel(void) { dispose(); }

    /**
     * Disposes all resources and assets of this room.
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a room may not be used until it is initialized again.
     */
    void dispose();

#pragma mark -
#pragma mark Accessors
    /**
     * Returns a shared pointer to the vector of physics objects that compose
     * the room geometry.
     * 
     * @return  Shared pointer to vector of physics objects for room geometry
     */
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> getPhysicsGeometry() { return _physicsGeometry; }

#pragma mark -
#pragma mark Animation
    /**
     * Draws the room with all of its interior geometry.
     * 
     * This overrides the draw() method of SceneNode to include drawing the room geometry.
     * 
     * @param batch     The SpriteBatch to draw with
     * @param transform The global affine transformation
     * @param tint      The tint to blend with the Node color
     */
    virtual void draw(const std::shared_ptr<SpriteBatch>& batch, const Affine2& transform, Color4 tint) override;

};

#endif /* MPRoomModel_h */