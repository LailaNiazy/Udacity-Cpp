#include "route_model.h"
#include <iostream>

RouteModel::RouteModel(const std::vector<std::byte> &xml) : Model(xml) {
    // Create RouteModel nodes.
    int counter = 0;
    for (Model::Node node : this->Nodes()) {
        //returns a vector of class Node
        m_Nodes.emplace_back(Node(counter, this, node));
        counter++;
    }
    CreateNodeToRoadHashmap();
}


void RouteModel::CreateNodeToRoadHashmap() {
    //output is a map with <int, vector<Model::Road*>> with node to the corrresponding road
    for (const Model::Road &road : Roads()) {
            //check that the road is not a footway
        if (road.type != Model::Road::Type::Footway) {
            //iterate over the nodes of each way of this road
            for (int node_idx : Ways()[road.way].nodes) {
                //check if node_idx is in the map of node_to_road. If not add it
                if (node_to_road.find(node_idx) == node_to_road.end()) {
                    node_to_road[node_idx] = std::vector<const Model::Road *> ();
                }
                //add the road to the map with type: <int, std::vector<const Model::Road *>>
                node_to_road[node_idx].push_back(&road);
            }
        }
    }
}


RouteModel::Node *RouteModel::Node::FindNeighbor(std::vector<int> node_indices) {
    Node *closest_node = nullptr;
    Node node;
    //input is the indices of the nodes
    for (int node_index : node_indices) {
        //intialize the node using the parent_model
        node = parent_model->SNodes()[node_index];
        //check if the distance is larger than 0 and if the node has been visited before
        if (this->distance(node) != 0 && !node.visited) {
            //check if the closest node is the nullptr and if the distance to the closest node is larger than the the current node
            if (closest_node == nullptr || this->distance(node) < this->distance(*closest_node)) {
                closest_node = &parent_model->SNodes()[node_index];
            }
        }
    }
    return closest_node;
}


void RouteModel::Node::FindNeighbors() {
    //process this vector: std::vector<Node *> neighbors -> result a vector with all the neighbors
    for (auto & road : parent_model->node_to_road[this->index]) {
        RouteModel::Node *new_neighbor = this->FindNeighbor(parent_model->Ways()[road->way].nodes);
        if (new_neighbor) {
            this->neighbors.emplace_back(new_neighbor);
        }
    }
}


RouteModel::Node &RouteModel::FindClosestNode(float x, float y) {
    Node input;
    input.x = x;
    input.y = y;

    float min_dist = std::numeric_limits<float>::max();
    float dist;
    int closest_idx;

    for (const Model::Road &road : Roads()) {
        if (road.type != Model::Road::Type::Footway) {
            for (int node_idx : Ways()[road.way].nodes) {
                dist = input.distance(SNodes()[node_idx]);
                if (dist < min_dist) {
                    closest_idx = node_idx;
                    min_dist = dist;
                }
            }
        }
    }

    return SNodes()[closest_idx];
}