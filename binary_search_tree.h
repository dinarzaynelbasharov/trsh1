#ifndef DATA_STRUCTURES_CPP_BINARY_SEARCH_TREE_H
#define DATA_STRUCTURES_CPP_BINARY_SEARCH_TREE_H

#include <list>
#include <stack>
#include "../../logger/logger.h"
#include "../../logger/logger_holder.h"
#include "../../allocator/allocator.h"
#include "../../allocator/allocator_holder.h"
#include "search_tree.h"

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
class binary_search_tree:
    public search_tree<tkey, tvalue, tkey_comparer>,
    private allocator_holder,
    private logger_holder
{

public:

protected:

    class template_method_basics:
        protected logger_holder
    {

        friend class binary_search_tree<tkey, tvalue, tkey_comparer>;

    private:

        binary_search_tree<tkey, tvalue, tkey_comparer> *_target_tree;

    public:

        explicit template_method_basics(
            binary_search_tree<tkey, tvalue, tkey_comparer> *target_tree)
            : _target_tree(target_tree)
        {

        }

    public:

        std::pair<std::stack<typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node **>, typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node **> find_path(
            tkey const &key)
        {
            std::stack<typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node **> path;

            if (_target_tree->_root == nullptr)
            {
                return { path, &_target_tree->_root };
            }

            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node **iterator = &_target_tree->_root;
            tkey_comparer comparer;

            while ((*iterator) != nullptr)
            {
                auto comparison_result = comparer(key, (*iterator)->keys_and_values[0].key);
                if (comparison_result == 0)
                {
                    return { path, iterator };
                }

                path.push(iterator);
                iterator = comparison_result < 0
                           ? (*iterator)->subtrees
                           : (*iterator)->subtrees + 1;
            }

            return { path, iterator };
        }

    private:

        [[nodiscard]] logger *get_logger() const noexcept override
        {
            return _target_tree->get_logger();
        }

    };

    class insertion_template_method:
        protected search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context,
        protected template_method_basics
    {

    public:

        explicit insertion_template_method(
            binary_search_tree<tkey, tvalue, tkey_comparer> *target_tree)
            : template_method_basics(target_tree)
        {

        }

    public:

        void insert(
            tkey const &key,
            tvalue &&value)
        {
            auto path_and_target = this->find_path(key);
            auto path = path_and_target.first;
            auto **target_ptr = path_and_target.second;

            if (*target_ptr != nullptr)
            {
                // TODO: exception || update value
            }

            *target_ptr = this->create_node(1);

            (*target_ptr)->keys_and_values[0].key = key;
            (*target_ptr)->keys_and_values[0].value = std::move(value);

            after_insert_inner(path, target_ptr);
        }

    protected:

        virtual void after_insert_inner(
            std::stack<typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node **> &path,
            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node **target_ptr)
        {
            // TODO: nothing to do here in BST context...
        }

    private:

        allocator *get_allocator() const noexcept override
        {
            return this->_target_tree->_allocator;
        }

    };

    class finding_template_method :
        public template_method_basics
    {

    public:

        explicit finding_template_method(
            binary_search_tree<tkey, tvalue, tkey_comparer> *target_tree)
            : template_method_basics(target_tree)
        {

        }

    public:

        tvalue const &find(
            tkey const &key)
        {
            auto path_and_target = this->find_path(key);
            auto path = path_and_target.first;
            auto **target_ptr = path_and_target.second;

            if (*target_ptr == nullptr)
            {
                // TODO: exception
            }

            after_find_inner(path, target_ptr);

            return (*target_ptr)->keys_and_values[0].value;
        }

    protected:

        virtual void after_find_inner(
            std::stack<typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node **> &path,
            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node **target_ptr)
        {
            // TODO: nothing to do here in BST context...
        }

    };

    class removing_template_method:
        public template_method_basics,
        private allocator_holder
    {

    public:

        explicit removing_template_method(
            binary_search_tree<tkey, tvalue, tkey_comparer> *target_tree)
            : template_method_basics(target_tree)
        {

        }

    public:

        tvalue &&remove(
            tkey const &key)
        {
            auto path_and_target = this->find_path(key);
            auto path = path_and_target.first;
            auto **target_ptr = path_and_target.second;

            if (*target_ptr == nullptr)
            {
                // TODO: exception
            }

            tvalue &&result = std::move((*target_ptr)->keys_and_values[0].value);

            if ((*target_ptr)->subtrees[0] != nullptr &&
                (*target_ptr)->subtrees[1] != nullptr)
            {
                auto **element_to_swap_with = &(*target_ptr)->subtrees[0];
                std::list<typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node **> subpath;
                subpath.push_back(target_ptr);

                while ((*element_to_swap_with)->subtrees[1] != nullptr)
                {
                    subpath.push_back(element_to_swap_with);
                    element_to_swap_with = &(*element_to_swap_with)->subtrees[1];
                }

                swap_nodes(element_to_swap_with, target_ptr);
            }

            if ((*target_ptr)->subtrees[0] == nullptr &&
                (*target_ptr)->subtrees[1] == nullptr)
            {
                cleanup_node(target_ptr);
            }
            else if ((*target_ptr)->subtrees[0] != nullptr)
            {
                auto *target_left_subtree = (*target_ptr)->subtrees[0];
                cleanup_node(target_ptr);
                *target_ptr = target_left_subtree;
            }
            else
            {
                auto *target_right_subtree = (*target_ptr)->subtrees[1];
                cleanup_node(target_ptr);
                *target_ptr = target_right_subtree;
            }

            after_remove(path);

            return std::move(result);
        }

    private:

        template<
            typename T>
        void swap(
            T **left,
            T **right)
        {
            T *temp = *left;
            *left = *right;
            *right = temp;
        }

        void swap_nodes(
            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node **one_node,
            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node **another_node)
        {
            swap(&(*one_node)->subtrees[0], &(*another_node)->subtrees[0]);
            swap(&(*one_node)->subtrees[1], &(*another_node)->subtrees[1]);

            swap_additional_data(*one_node, *another_node);

            swap(one_node, another_node);
        }

        void cleanup_node(
            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node **node_address)
        {
            (*node_address)->~search_tree_node();
            deallocate_with_guard(reinterpret_cast<void *>(*node_address));

            *node_address = nullptr;
        }

    protected:

        virtual void swap_additional_data(
            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *one_node,
            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *another_node)
        {

        }

        virtual void after_remove(
            std::stack<typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node **> const &path)
        {

        }

    private:

        allocator *get_allocator() const noexcept override
        {
            return this->_target_tree->_allocator;
        }

    };

private:

    typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *_root;
    logger *_logger;
    allocator *_allocator;
    insertion_template_method *_insertion;
    finding_template_method *_finding;
    removing_template_method *_removing;

public:

    binary_search_tree(
        binary_search_tree<tkey, tvalue, tkey_comparer> const &obj)
        : binary_search_tree(obj._logger, obj._allocator)
    {
        _root = copy(obj._root);
    }

    binary_search_tree(
        binary_search_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
        : binary_search_tree(obj._insertion,
                             obj._finding,
                             obj._removing,
                             obj._allocator,
                             obj._logger)
    {
        _root = obj._root;
        obj._root = nullptr;

        _insertion->_target_tree = this;
        obj._insertion = nullptr;

        _finding->_target_tree = this;
        obj._finding = nullptr;

        _removing->_target_tree = this;
        obj._removing = nullptr;

        obj._allocator = nullptr;

        obj._logger = nullptr;
    }

    binary_search_tree &operator=(
        binary_search_tree<tkey, tvalue, tkey_comparer> const &obj)
    {
        if (this == &obj)
        {
            return *this;
        }

        clearup(_root);

        _allocator = obj._allocator;
        _logger = obj._logger;

        _root = copy(obj._root);

        return *this;
    }

    binary_search_tree &operator=(
        binary_search_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
    {
        if (this == &obj)
        {
            return *this;
        }

        clearup(_root);
        _root = obj._root;
        obj._root = nullptr;

        delete obj._insertion;
        obj._insertion = nullptr;

        delete obj._finding;
        obj._finding = nullptr;

        delete obj._removing;
        obj._removing = nullptr;

        _allocator = obj._allocator;
        obj._allocator = nullptr;

        _logger = obj._logger;
        obj._logger = nullptr;

        return *this;
    }

    ~binary_search_tree()
    {
        delete _insertion;
        delete _finding;
        delete _removing;

        clearup(_root);
    }

private:

    void clearup(
        typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *element)
    {
        if (element == nullptr)
        {
            return;
        }

        clearup(element->subtrees[0]);
        clearup(element->subtrees[1]);


        deallocate_with_guard(element);
    }

    // TODO: think about usability in derived classes
    typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *copy(
        typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *from)
    {
        if (from == nullptr)
        {
            return nullptr;
        }

        typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *result = allocate_with_guard(sizeof(typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node));
        new (result) typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node(*from);

        result->left_subtree = copy(from->left_subtree);
        result->right_subtree = copy(from->right_subtree);

        return result;
    }

protected:

    binary_search_tree(
        logger *logger,
        allocator *allocator,
        insertion_template_method *insertion,
        finding_template_method *finding,
        removing_template_method *removing)
        : _logger(logger),
          _allocator(allocator),
          _insertion(insertion),
          _finding(finding),
          _removing(removing),
          _root(nullptr)
    {

    }

public:

    explicit binary_search_tree(
        logger *logger = nullptr,
        allocator *allocator = nullptr)
        : binary_search_tree(logger,
                             allocator,
                             new insertion_template_method(this),
                             new finding_template_method(this),
                             new removing_template_method(this))
    {

    }

public:

    void insert(
        tkey const &key,
        tvalue &&value) override
    {
        _insertion->insert(key, std::move(value));
    }

    tvalue const &get(
        tkey const &key) override
    {
        return _finding->find(key);
    }

    tvalue &&remove(
        tkey const &key) override
    {
        return std::move(_removing->remove(key));
    }

private:

    allocator *get_memory() const noexcept override
    {
        return _allocator;
    }

private:

    logger *get_logger() const noexcept override
    {
        return _logger;
    }

};


#endif //DATA_STRUCTURES_CPP_BINARY_SEARCH_TREE_H
