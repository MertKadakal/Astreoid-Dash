#include "CelestialObject.h"
#include "AsteroidDash.h"

// Constructor to initialize CelestialObject with essential properties
CelestialObject::CelestialObject(const vector<vector<bool>> &shape, ObjectType type, int start_row,
                                 int time_of_appearance)
        : shape(shape), object_type(type), starting_row(start_row), time_of_appearance(time_of_appearance) {

    appeared = false;
    original_shape = shape;

    CelestialObject* curr = this;

    // create the horizontal linkedlist
    while (get_right_rotation(curr->shape) != this->shape) {
        CelestialObject* toAdd = new CelestialObject(curr);
        toAdd->shape = get_right_rotation(curr->shape);
        curr->right_rotation = toAdd;
        toAdd->left_rotation = curr;
        curr = curr->right_rotation;
    }

    // link the last element to the first element to make linklist circular
    curr->right_rotation = this;
    this->left_rotation = curr;

}

// deletes rotations and re-create new rotations for updated object
void CelestialObject::create_rotations_again() {
    delete_rotations(this);

    CelestialObject* curr = this;

    while (get_right_rotation(curr->shape) != this->shape) {
        CelestialObject* toAdd = new CelestialObject(curr);
        toAdd->shape = get_right_rotation(curr->shape);
        curr->right_rotation = toAdd;
        toAdd->left_rotation = curr;
        curr = curr->right_rotation;
    }

    curr->right_rotation = this;
    this->left_rotation = curr;
}

// Copy constructor for CelestialObject
CelestialObject::CelestialObject(const CelestialObject *other)
        : shape(other->shape),  // Copy the 2D vector shape
          object_type(other->object_type),  // Copy the object type
          starting_row(other->starting_row),  // Copy the starting row
          time_of_appearance(other->time_of_appearance)  // Copy the time of appearance
{

}

vector<vector<bool>> CelestialObject::get_right_rotation(vector<vector<bool>> original) {

    int rows = original.size();
    int cols = original[0].size();

    std::vector<std::vector<bool>> rotated(cols, std::vector<bool>(rows));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            rotated[j][rows - i - 1] = original[i][j];
        }
    }

    return rotated;
}

vector<vector<bool>> CelestialObject::get_left_rotation(vector<vector<bool>> original) {

    int rows = original.size();
    int cols = original[0].size();

    std::vector<std::vector<bool>> rotated(cols, std::vector<bool>(rows));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            rotated[cols - j - 1][i] = original[i][j];
        }
    }

    return rotated;
}

// Function to delete rotations of a given celestial object. It should free the dynamically allocated
// memory for each rotation.
void CelestialObject::delete_rotations(CelestialObject *target) {
    CelestialObject* curr = target->right_rotation;
    while (curr != target && curr!=nullptr) {
        CelestialObject* next = curr->right_rotation;
        delete curr;
        curr = next;
    }
}
