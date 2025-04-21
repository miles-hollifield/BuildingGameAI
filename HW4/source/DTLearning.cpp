/**
 * @file DTLearning.cpp
 * @brief Implementation of the Decision Tree Learning algorithm.
 *
 * Resources Used:
 * - Book: "Artificial Intelligence for Games" by Ian Millington
 * - AI Tools: OpenAI's ChatGPT
 *
 * Author: Miles Hollifield
 * Date: 4/7/2025
 */

#include "headers/DTLearning.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <map>

// DTLeafNode implementation
std::string DTLeafNode::toString(int indent) const
{
    std::string indentStr(indent * 2, ' ');
    return indentStr + "LEAF: " + label;
}

// DTInternalNode implementation
std::string DTInternalNode::classify(const std::vector<std::string> &dataPoint) const
{
    // Get the value of the attribute we're splitting on
    std::string attributeValue = dataPoint[attributeIndex];

    // Find the matching child
    auto it = children.find(attributeValue);
    if (it != children.end())
    {
        // If found, delegate to that child
        return it->second->classify(dataPoint);
    }
    else
    {
        // If not found, find the first child as a fallback
        if (!children.empty())
        {
            return children.begin()->second->classify(dataPoint);
        }
        // This should never happen
        return "Unknown";
    }
}

std::string DTInternalNode::toString(int indent) const
{
    std::string indentStr(indent * 2, ' ');
    std::string result = indentStr + "SPLIT ON: " + attributeName + "\n";

    for (const auto &child : children)
    {
        result += indentStr + "  " + attributeName + " = " + child.first + ":\n";
        result += child.second->toString(indent + 2) + "\n";
    }

    // Remove the last newline
    if (!result.empty())
    {
        result.pop_back();
    }

    return result;
}

// DecisionTreeLearner implementation
DecisionTreeLearner::DecisionTreeLearner() : rootNode(nullptr)
{
}

/**
 * @brief Load data from a CSV file
 * * @param filename Name of the file to load
 * * @param skipHeader Whether to skip the first line (header)
 * * @return True if successful, false otherwise
 *
 * OpenAI's ChatGPT was used to suggest a template for this function.
 * The following prompt was used:
 * "Create a function to load data from a CSV file for decision tree learning in C++.
 * The data should include attributes like DistanceToPlayer, RelativeOrientation, Speed, etc."
 * The code provided by ChatGPT was modified to fit the context of the project.
 */
bool DecisionTreeLearner::loadData(const std::string &filename, bool skipHeader)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    data.clear();
    std::string line;

    // Skip header if needed
    if (skipHeader && std::getline(file, line))
    {
        // If we have no attribute names yet, parse them from the header
        if (attributeNames.empty())
        {
            std::istringstream iss(line);
            std::string token;
            while (std::getline(iss, token, ','))
            {
                attributeNames.push_back(token);
            }
            // Remove the last one, which is the label
            if (!attributeNames.empty())
            {
                attributeNames.pop_back();
            }
        }
    }

    // Read data and discretize numeric values
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string token;
        DataPoint point;

        // Parse attributes
        int attrIndex = 0;
        while (std::getline(iss, token, ','))
        {
            if (iss.peek() != EOF)
            {
                // Discretize numeric attributes based on their index
                if (attrIndex == 0) // DistanceToPlayer
                {
                    try
                    {
                        float distance = std::stof(token);
                        if (distance < 30.0f)
                            point.attributes.push_back("very_near");
                        else if (distance < 80.0f)
                            point.attributes.push_back("near");
                        else if (distance < 200.0f)
                            point.attributes.push_back("medium");
                        else
                            point.attributes.push_back("far");
                    }
                    catch (...)
                    {
                        point.attributes.push_back(token); // Keep original if not a number
                    }
                }
                else if (attrIndex == 1) // RelativeOrientation
                {
                    try
                    {
                        float orientation = std::stof(token);
                        if (std::abs(orientation) < 30.0f)
                            point.attributes.push_back("direct_front");
                        else if (std::abs(orientation) < 90.0f)
                            point.attributes.push_back("front");
                        else if (std::abs(orientation) < 150.0f)
                            point.attributes.push_back("side");
                        else
                            point.attributes.push_back("behind");
                    }
                    catch (...)
                    {
                        point.attributes.push_back(token);
                    }
                }
                else if (attrIndex == 2) // Speed
                {
                    try
                    {
                        float speed = std::stof(token);
                        if (speed < 5.0f)
                            point.attributes.push_back("stopped");
                        else if (speed < 50.0f)
                            point.attributes.push_back("very_slow");
                        else if (speed < 100.0f)
                            point.attributes.push_back("slow");
                        else if (speed < 150.0f)
                            point.attributes.push_back("medium_speed");
                        else
                            point.attributes.push_back("fast");
                    }
                    catch (...)
                    {
                        point.attributes.push_back(token);
                    }
                }
                else if (attrIndex == 3 || attrIndex == 4) // CanSeePlayer, IsNearObstacle (binary)
                {
                    point.attributes.push_back(token);
                }
                else if (attrIndex == 5) // PathCount
                {
                    try
                    {
                        int count = std::stoi(token);
                        if (count == 0)
                            point.attributes.push_back("none");
                        else if (count < 3)
                            point.attributes.push_back("very_few");
                        else if (count < 7)
                            point.attributes.push_back("few");
                        else if (count < 15)
                            point.attributes.push_back("medium");
                        else
                            point.attributes.push_back("many");
                    }
                    catch (...)
                    {
                        point.attributes.push_back(token);
                    }
                }
                else if (attrIndex == 6) // TimeInCurrentAction
                {
                    try
                    {
                        float time = std::stof(token);
                        if (time < 0.5f)
                            point.attributes.push_back("very_short");
                        else if (time < 1.5f)
                            point.attributes.push_back("short");
                        else if (time < 3.0f)
                            point.attributes.push_back("medium");
                        else if (time < 5.0f)
                            point.attributes.push_back("long");
                        else
                            point.attributes.push_back("very_long");
                    }
                    catch (...)
                    {
                        point.attributes.push_back(token);
                    }
                }
                else
                {
                    point.attributes.push_back(token); // Keep other attributes as-is
                }
                attrIndex++;
            }
            else
            {
                // Last token is the label (action)
                point.label = token;
                break;
            }
        }

        // Validation to ensure we have a complete data point
        if (!point.attributes.empty() && !point.label.empty())
        {
            data.push_back(point);
        }
    }

    // Print some statistics for debugging
    std::map<std::string, int> labelCounts;
    for (const auto &example : data)
    {
        labelCounts[example.label]++;
    }

    std::cout << "Loaded " << data.size() << " data points with " << labelCounts.size() << " different actions:" << std::endl;
    for (const auto &count : labelCounts)
    {
        std::cout << "  - " << count.first << ": " << count.second << " examples" << std::endl;
    }

    return !data.empty();
}

void DecisionTreeLearner::setAttributeNames(const std::vector<std::string> &names)
{
    attributeNames = names;
}

std::shared_ptr<DTNode> DecisionTreeLearner::learnTree()
{
    // Initialize the list of attributes to consider
    std::vector<int> attributes;
    for (size_t i = 0; i < (data.empty() ? 0 : data[0].attributes.size()); i++)
    {
        attributes.push_back(i);
    }

    // Build the tree recursively
    rootNode = buildTree(data, attributes, data);
    return rootNode;
}

bool DecisionTreeLearner::saveTree(const std::string &filename) const
{
    if (!rootNode)
    {
        std::cerr << "No tree to save" << std::endl;
        return false;
    }

    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }

    // Write attribute names
    for (size_t i = 0; i < attributeNames.size(); i++)
    {
        file << attributeNames[i];
        if (i < attributeNames.size() - 1)
        {
            file << ",";
        }
    }
    file << std::endl;

    // Write the tree
    file << rootNode->toString() << std::endl;

    return true;
}

bool DecisionTreeLearner::loadTree(const std::string &filename)
{
    // TODO: Implement tree loading from file
    std::cerr << "Tree loading not implemented" << std::endl;
    return false;
}

std::string DecisionTreeLearner::classify(const std::vector<std::string> &dataPoint) const
{
    if (!rootNode)
    {
        return "Unknown";
    }

    return rootNode->classify(dataPoint);
}

std::shared_ptr<DTNode> DecisionTreeLearner::getTree() const
{
    return rootNode;
}

std::string DecisionTreeLearner::printTree() const
{
    if (!rootNode)
    {
        return "No tree learned yet";
    }

    return rootNode->toString();
}

/**
 * @brief Recursively build the decision tree
 * @param examples Current set of examples to consider
 * @param attributes Remaining attributes to consider for splitting
 * @param parentExamples Examples from the parent node for majority voting
 * @return Root node of the subtree
 *
 * OpenAI's ChatGPT was used to help implement this function.
 * The following prompt was used:
 * "Implement a recursive function to build a decision tree
 * from a set of examples. The function should handle cases
 * where examples are empty, all examples have the same label,
 * and when no attributes provide sufficient information gain."
 * The code provided by ChatGPT was modified to fit the context of the project.
 */
std::shared_ptr<DTNode> DecisionTreeLearner::buildTree(
    const std::vector<DataPoint> &examples,
    const std::vector<int> &attributes,
    const std::vector<DataPoint> &parentExamples)
{
    // If examples is empty, return the majority label from parent examples
    if (examples.empty())
    {
        return std::make_shared<DTLeafNode>(getMajorityLabel(parentExamples));
    }

    // If all examples have the same label, return a leaf node with that label
    if (allSameLabel(examples))
    {
        return std::make_shared<DTLeafNode>(examples[0].label);
    }

    // If attributes is empty, return a leaf node with the majority label
    if (attributes.empty())
    {
        return std::make_shared<DTLeafNode>(getMajorityLabel(examples));
    }

    // Minimum information gain threshold to avoid splits with limited value
    const double MIN_GAIN_THRESHOLD = 0.01;

    // Find the attribute with the highest information gain
    int bestAttributeIndex = -1;
    double bestGain = MIN_GAIN_THRESHOLD; // Must exceed this threshold

    for (int attributeIndex : attributes)
    {
        double gain = calculateInformationGain(examples, attributeIndex);
        if (gain > bestGain)
        {
            bestGain = gain;
            bestAttributeIndex = attributeIndex;
        }
    }

    // If no attribute provides sufficient information gain, return a leaf node
    if (bestAttributeIndex == -1)
    {
        return std::make_shared<DTLeafNode>(getMajorityLabel(examples));
    }

    // Create a new decision node
    std::string attributeName = (bestAttributeIndex < attributeNames.size()) ? attributeNames[bestAttributeIndex] : "Attribute " + std::to_string(bestAttributeIndex);
    auto node = std::make_shared<DTInternalNode>(bestAttributeIndex, attributeName);

    // Create a new list of attributes without the best attribute
    std::vector<int> remainingAttributes;
    for (int attr : attributes)
    {
        if (attr != bestAttributeIndex)
        {
            remainingAttributes.push_back(attr);
        }
    }

    // For each possible value of the best attribute
    auto attributeValues = getAttributeValues(examples, bestAttributeIndex);
    for (const auto &value : attributeValues)
    {
        // Create a subset of examples with this attribute value
        std::vector<DataPoint> subExamples;
        for (const auto &example : examples)
        {
            if (example.attributes[bestAttributeIndex] == value)
            {
                subExamples.push_back(example);
            }
        }

        // Add a minimum example threshold to avoid overfitting
        const int MIN_EXAMPLES_FOR_SPLIT = 3;

        if (subExamples.size() < MIN_EXAMPLES_FOR_SPLIT)
        {
            // Too few examples for this value, use majority class from parent
            node->addChild(value, std::make_shared<DTLeafNode>(getMajorityLabel(examples)));
        }
        else
        {
            // Recursively build the subtree
            auto subtree = buildTree(subExamples, remainingAttributes, examples);

            // Add the subtree to the decision node
            node->addChild(value, subtree);
        }
    }

    return node;
}

double DecisionTreeLearner::calculateEntropy(const std::vector<DataPoint> &examples) const
{
    if (examples.empty())
    {
        return 0.0;
    }

    // Count occurrences of each label
    std::map<std::string, int> labelCounts;
    for (const auto &example : examples)
    {
        labelCounts[example.label]++;
    }

    // Calculate entropy
    double entropy = 0.0;
    double numExamples = examples.size();

    for (const auto &count : labelCounts)
    {
        double probability = count.second / numExamples;
        entropy -= probability * std::log2(probability);
    }

    return entropy;
}

double DecisionTreeLearner::calculateInformationGain(
    const std::vector<DataPoint> &examples,
    int attributeIndex) const
{

    double entropyBefore = calculateEntropy(examples);
    double entropyAfter = 0.0;
    double numExamples = examples.size();

    // Group examples by attribute value
    std::map<std::string, std::vector<DataPoint>> valueGroups;
    for (const auto &example : examples)
    {
        valueGroups[example.attributes[attributeIndex]].push_back(example);
    }

    // Calculate weighted entropy after split
    for (const auto &group : valueGroups)
    {
        double weight = group.second.size() / numExamples;
        entropyAfter += weight * calculateEntropy(group.second);
    }

    return entropyBefore - entropyAfter;
}

std::string DecisionTreeLearner::getMajorityLabel(const std::vector<DataPoint> &examples) const
{
    if (examples.empty())
    {
        return "Unknown";
    }

    // Count occurrences of each label
    std::map<std::string, int> labelCounts;
    for (const auto &example : examples)
    {
        labelCounts[example.label]++;
    }

    // Find the most common label
    std::string majorityLabel = examples[0].label;
    int maxCount = 0;

    for (const auto &count : labelCounts)
    {
        if (count.second > maxCount)
        {
            maxCount = count.second;
            majorityLabel = count.first;
        }
    }

    return majorityLabel;
}

bool DecisionTreeLearner::allSameLabel(const std::vector<DataPoint> &examples) const
{
    if (examples.empty())
    {
        return true;
    }

    std::string firstLabel = examples[0].label;
    for (const auto &example : examples)
    {
        if (example.label != firstLabel)
        {
            return false;
        }
    }

    return true;
}

std::set<std::string> DecisionTreeLearner::getAttributeValues(
    const std::vector<DataPoint> &examples,
    int attributeIndex) const
{

    std::set<std::string> values;
    for (const auto &example : examples)
    {
        values.insert(example.attributes[attributeIndex]);
    }

    return values;
}