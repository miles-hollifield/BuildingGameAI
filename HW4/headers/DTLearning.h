/**
 * @file DTLearning.h
 * @brief Defines the Decision Tree Learning algorithm.
 *
 * Resources Used:
 * - Book: "Machine Learning" by Tom Mitchell
 * - Book: "Artificial Intelligence: A Modern Approach" by Stuart Russell and Peter Norvig
 *
 * Author: Miles Hollifield
 * Date: 4/7/2025
 */

#ifndef DT_LEARNING_H
#define DT_LEARNING_H

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <map>
#include <set>

/**
 * @struct DataPoint
 * @brief Represents a single data point for decision tree learning
 */
struct DataPoint
{
    std::vector<std::string> attributes; // Categorical attributes
    std::string label;                   // Class label
};

/**
 * @class DecisionTreeNode
 * @brief Node in the learned decision tree
 */
class DTNode
{
public:
    virtual ~DTNode() = default;

    /**
     * @brief Classify a data point
     * @param dataPoint Data point to classify
     * @return Predicted label
     */
    virtual std::string classify(const std::vector<std::string> &dataPoint) const = 0;

    /**
     * @brief Print the node to a string with indentation
     * @param indent Indentation level
     * @return String representation of the node
     */
    virtual std::string toString(int indent = 0) const = 0;
};

/**
 * @class DTLeafNode
 * @brief Leaf node in the learned decision tree
 */
class DTLeafNode : public DTNode
{
public:
    /**
     * @brief Constructor
     * @param label The class label for this leaf
     */
    DTLeafNode(const std::string &label) : label(label) {}

    /**
     * @brief Classify a data point
     * @param dataPoint Data point to classify
     * @return Predicted label
     */
    std::string classify(const std::vector<std::string> &dataPoint) const override
    {
        return label;
    }

    /**
     * @brief Print the node to a string with indentation
     * @param indent Indentation level
     * @return String representation of the node
     */
    std::string toString(int indent = 0) const override;

private:
    std::string label;
};

/**
 * @class DTInternalNode
 * @brief Internal node in the learned decision tree
 */
class DTInternalNode : public DTNode
{
public:
    /**
     * @brief Constructor
     * @param attributeIndex Index of the attribute to split on
     * @param attributeName Name of the attribute to split on
     */
    DTInternalNode(int attributeIndex, const std::string &attributeName)
        : attributeIndex(attributeIndex), attributeName(attributeName) {}

    /**
     * @brief Add a child node for a specific attribute value
     * @param attributeValue Value of the attribute
     * @param child Child node
     */
    void addChild(const std::string &attributeValue, std::shared_ptr<DTNode> child)
    {
        children[attributeValue] = child;
    }

    /**
     * @brief Classify a data point
     * @param dataPoint Data point to classify
     * @return Predicted label
     */
    std::string classify(const std::vector<std::string> &dataPoint) const override;

    /**
     * @brief Print the node to a string with indentation
     * @param indent Indentation level
     * @return String representation of the node
     */
    std::string toString(int indent = 0) const override;

private:
    int attributeIndex;
    std::string attributeName;
    std::unordered_map<std::string, std::shared_ptr<DTNode>> children;
};

/**
 * @class DecisionTreeLearner
 * @brief Implementation of the ID3 decision tree learning algorithm
 */
class DecisionTreeLearner
{
public:
    /**
     * @brief Constructor
     */
    DecisionTreeLearner();

    /**
     * @brief Load data from a CSV file
     * @param filename CSV file to load
     * @param skipHeader Whether to skip the first line (header)
     * @return True if successful, false otherwise
     */
    bool loadData(const std::string &filename, bool skipHeader = true);

    /**
     * @brief Set the attribute names
     * @param names Vector of attribute names
     */
    void setAttributeNames(const std::vector<std::string> &names);

    /**
     * @brief Learn a decision tree from the loaded data
     * @return Root node of the learned decision tree
     */
    std::shared_ptr<DTNode> learnTree();

    /**
     * @brief Save the learned tree to a file
     * @param filename File to save to
     * @return True if successful, false otherwise
     */
    bool saveTree(const std::string &filename) const;

    /**
     * @brief Load a tree from a file
     * @param filename File to load from
     * @return True if successful, false otherwise
     */
    bool loadTree(const std::string &filename);

    /**
     * @brief Classify a new data point
     * @param dataPoint Data point to classify
     * @return Predicted label
     */
    std::string classify(const std::vector<std::string> &dataPoint) const;

    /**
     * @brief Get the root node of the learned tree
     * @return Root node of the learned tree
     */
    std::shared_ptr<DTNode> getTree() const;

    /**
     * @brief Print the tree
     * @return String representation of the tree
     */
    std::string printTree() const;

private:
    std::vector<DataPoint> data;
    std::shared_ptr<DTNode> rootNode;
    std::vector<std::string> attributeNames;

    /**
     * @brief Recursively build the decision tree
     * @param examples Subset of data points to consider
     * @param attributes Indices of attributes to consider
     * @param parentExamples Parent examples for majority voting
     * @return Root node of the subtree
     */
    std::shared_ptr<DTNode> buildTree(
        const std::vector<DataPoint> &examples,
        const std::vector<int> &attributes,
        const std::vector<DataPoint> &parentExamples);

    /**
     * @brief Calculate the entropy of a set of examples
     * @param examples Set of examples to calculate entropy for
     * @return Entropy value
     */
    double calculateEntropy(const std::vector<DataPoint> &examples) const;

    /**
     * @brief Calculate the information gain for an attribute
     * @param examples Set of examples to calculate gain for
     * @param attributeIndex Index of the attribute to calculate gain for
     * @return Information gain value
     */
    double calculateInformationGain(
        const std::vector<DataPoint> &examples,
        int attributeIndex) const;

    /**
     * @brief Get the most common label in a set of examples
     * @param examples Set of examples to find the most common label for
     * @return Most common label
     */
    std::string getMajorityLabel(const std::vector<DataPoint> &examples) const;

    /**
     * @brief Check if all examples have the same label
     * @param examples Set of examples to check
     * @return True if all examples have the same label, false otherwise
     */
    bool allSameLabel(const std::vector<DataPoint> &examples) const;

    /**
     * @brief Get all possible values for an attribute
     * @param examples Set of examples to consider
     * @param attributeIndex Index of the attribute
     * @return Set of possible values
     */
    std::set<std::string> getAttributeValues(
        const std::vector<DataPoint> &examples,
        int attributeIndex) const;
};

#endif // DT_LEARNING_H