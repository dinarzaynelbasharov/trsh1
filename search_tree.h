#ifndef DATA_STRUCTURES_CPP_SEARCH_TREE_H
#define DATA_STRUCTURES_CPP_SEARCH_TREE_H

#include <exception>
#include <optional>
#include "../associative_container.h"
#include "../../exception/not_implemented.h"
#include "../../logger/logger.h"
#include "../../logger/logger_holder.h"
#include "../../allocator/allocator.h"
#include "../../allocator/allocator_holder.h"

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
class search_tree:
    public associative_container<tkey, tvalue>,
    protected allocator_holder,
    protected logger_holder
{

protected:

    struct search_tree_node
    {

        typename associative_container<tkey, tvalue>::key_value_pair *keys_and_values;
        search_tree_node **subtrees;
        unsigned int involved_keys {};
        unsigned int min_keys_count {};

    public:

        virtual ~search_tree_node() noexcept = default;

    };

protected:

    class node_interaction_context:
        public allocator_holder,
        public logger_holder
    {

    public:

        enum class node_split_mode
        {
            do_not_copy_median_kvp = 0,
            copy_median_kvp = 1
        };

    private:

        logger *_logger;
        allocator *_allocator;

    public:

        node_interaction_context(
            logger *logger,
            allocator *allocator)
            : _logger(logger),
              _allocator(allocator)
        {

        }

    public:

        virtual typename search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_split_mode get_split_mode() const;

        typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *create_node_shell(
            unsigned int key_value_pairs_min_count);

        typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *copy_node(
            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *node_shell,
            bool recursive);

        typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *copy_node_inner(
            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *node_shell,
            bool recursive,
            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *&last_added_leaf,
            typename search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_split_mode split_mode);

        void destroy_node(
            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *node_shell,
            bool recursive);

        std::optional<std::pair<search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *, typename associative_container<tkey, tvalue>::key_value_pair> > shift(
            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *node,
            unsigned int index_to_vacate,
            tkey const &key,
            tvalue &&value,
            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *vacation_right_subtree);

    protected:

        [[nodiscard]] virtual size_t get_node_shell_size() const noexcept;

        virtual void node_constructor_call(
            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *allocated_memory) const noexcept;

        virtual void node_additional_data_injector(
            typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *initialized_memory) const noexcept;

    protected:

        [[nodiscard]] allocator *get_allocator() const noexcept final;

    protected:

        [[nodiscard]] logger *get_logger() const noexcept final;

    };

public:

    enum class insertion_of_existing_key_mode
    {
        throw_an_exception,
        update_value
    };

    enum class removing_of_node_with_two_subtrees_swap_mode
    {
        with_max_from_left_subtree,
        with_min_from_right_subtree
    };

public:

    class insertion_exception final:
        public std::exception
    {

    public:

        [[nodiscard]] char const *what() const noexcept override;

    };

    class reading_exception final:
        public std::exception
    {

    public:

        [[nodiscard]] char const *what() const noexcept override;

    };

    class removing_exception final:
        public std::exception
    {

    public:

        [[nodiscard]] char const *what() const noexcept override;

    };

protected:

    search_tree_node *_root;
    node_interaction_context *_context;

private:

    logger *_logger;
    allocator *_allocator;

protected:

    explicit search_tree(
        node_interaction_context *context,
        allocator *allocator,
        logger *logger)
        : _context(context),
          _allocator(allocator),
          _logger(logger),
          _root(nullptr)
    {

    }

public:

    search_tree(search_tree<tkey, tvalue, tkey_comparer> const &other);

    search_tree<tkey, tvalue, tkey_comparer> &operator=(search_tree<tkey, tvalue, tkey_comparer> const &other);

    search_tree(search_tree<tkey, tvalue, tkey_comparer> &&other) noexcept;

    search_tree<tkey, tvalue, tkey_comparer> &operator=(search_tree<tkey, tvalue, tkey_comparer> &&other) noexcept;

    ~search_tree();

protected:

    virtual node_interaction_context *create_node_interaction_context(
        logger *logger,
        allocator *allocator) const noexcept
    {
        return new node_interaction_context(logger, allocator);
    }

protected:

    [[nodiscard]] allocator *get_allocator() const noexcept final;

protected:

    [[nodiscard]] logger *get_logger() const noexcept final;

};

// region search_tree<tkey, tvalue, tkey_comparer> implementation

// region search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context implementation

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
typename search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_split_mode search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::get_split_mode() const
{
    throw not_implemented("node split mode not defined");
}

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::create_node_shell(
    unsigned int key_value_pairs_min_count)
{
    auto key_value_pairs_max_count = key_value_pairs_min_count << 1;

    auto *node_shell = reinterpret_cast<search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *>(allocate_with_guard(get_node_shell_size()));
    new (node_shell) typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node;

    node_shell->keys_and_values = reinterpret_cast<typename associative_container<tkey, tvalue>::key_value_pair *>(allocate_with_guard(sizeof(typename associative_container<tkey, tvalue>::key_value_pair) * key_value_pairs_max_count));
    node_shell->subtrees = reinterpret_cast<search_tree<tkey, tvalue, tkey_comparer>::search_tree_node **>(allocate_with_guard(sizeof(typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *) * (key_value_pairs_max_count + 1)));
    node_shell->min_keys_count = key_value_pairs_min_count;
    node_shell->involved_keys = 0;

    return node_shell;
}

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::copy_node(
    typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *node_shell,
    bool recursive)
{
    typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *last_added_leaf = nullptr;
    auto *copied_node_shell = copy_node_inner(node_shell, recursive, last_added_leaf, get_split_mode());

    if (last_added_leaf != nullptr)
    {
        last_added_leaf->subtrees[last_added_leaf->involved_keys] = nullptr;
    }

    return copied_node_shell;
}

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::copy_node_inner(
    typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *node_shell,
    bool recursive,
    typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *&last_added_leaf,
    typename search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_split_mode split_mode)
{
    if (node_shell == nullptr)
    {
        return nullptr;
    }

    if (node_shell->subtrees[0] == nullptr && recursive)
    {
        return copy_node_inner(node_shell, false, last_added_leaf, split_mode);
    }

    auto *node_shell_copy = create_node_shell(node_shell->min_keys_count);
    node_shell_copy->involved_keys = node_shell->involved_keys;

    for (auto i = 0; i <= node_shell->involved_keys; i++)
    {
        if (i != node_shell->involved_keys)
        {
            new (node_shell_copy->keys_and_values + i) typename associative_container<tkey, tvalue>::key_value_pair(node_shell->keys_and_values[i]);
        }

        if (recursive)
        {
            switch (split_mode)
            {
                case search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_split_mode::do_not_copy_median_kvp:
                    node_shell_copy->subtrees[i] = copy_node_inner(node_shell->subtrees[i], recursive, last_added_leaf, split_mode);
                    break;
                case search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_split_mode::copy_median_kvp:
                    if (i != node_shell->involved_keys || node_shell->subtrees[0] != nullptr)
                    {
                        node_shell_copy->subtrees[i] = copy_node_inner(node_shell->subtrees[i], recursive, last_added_leaf, split_mode);
                    }
                    break;
            }
        }

        if (i == 0 && node_shell_copy->subtrees[0] == nullptr)
        {
            if (last_added_leaf != nullptr)
            {
                switch (split_mode)
                {
                    case search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_split_mode::do_not_copy_median_kvp:
                        last_added_leaf->subtrees[last_added_leaf->involved_keys] = nullptr;
                        break;
                    case search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_split_mode::copy_median_kvp:
                        last_added_leaf->subtrees[last_added_leaf->involved_keys] = node_shell_copy;
                        break;
                }
            }

            last_added_leaf = node_shell_copy;
        }
    }

    return node_shell_copy;
}

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
void search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::destroy_node(
    typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *node_shell,
    bool recursive)
{
    if (node_shell == nullptr)
    {
        return;
    }

    if (node_shell->subtrees[0] == nullptr && recursive)
    {
        destroy_node(node_shell, false);

        return;
    }

    if (recursive)
    {
        for (auto i = 0; i <= node_shell->involved_keys; i++)
        {
            destroy_node(node_shell->subtrees[i], true);
        }
    }
    
    deallocate_with_guard(node_shell->subtrees);
    for (auto i = 0; i < node_shell->involved_keys; i++)
    {
        node_shell->keys_and_values[i].~key_value_pair();
    }

    deallocate_with_guard(node_shell->keys_and_values);
    node_shell->~search_tree_node();
    deallocate_with_guard(node_shell);
}

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
std::optional<std::pair<typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *, typename associative_container<tkey, tvalue>::key_value_pair> > search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::shift(
    typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *node,
    unsigned int index_to_vacate,
    tkey const &key,
    tvalue &&value,
    typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *vacation_right_subtree)
{
    auto split_mode = get_split_mode();
    auto node_max_keys_count = node->min_keys_count << 1;
    if (node->involved_keys == node_max_keys_count)
    {
        if (split_mode == search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_split_mode::copy_median_kvp && node->subtrees[0] != nullptr)
        {
            split_mode = search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_split_mode::do_not_copy_median_kvp;
        }

        auto additional_size = node->subtrees[0] == nullptr
            ? static_cast<int>(split_mode)
            : 0;
        auto *new_node_after_split = create_node_shell(node->min_keys_count);

        new_node_after_split->involved_keys = (node->involved_keys >>= 1) + additional_size;

        typename associative_container<tkey, tvalue>::key_value_pair kvps[node_max_keys_count + 1];
        for (auto i = 0; i <= node_max_keys_count; i++)
        {
            if (i == index_to_vacate)
            {
                kvps[i].key = key;
                kvps[i].value = std::move(value);
            }
            else
            {
                kvps[i] = std::move(node->keys_and_values[i - (i < index_to_vacate
                    ? 0
                    : 1)]);
            }
        }

        typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node* subtrees[node_max_keys_count + 2];
        for (auto i = 0; i <= node_max_keys_count; i++)
        {
            if (i == index_to_vacate)
            {
                subtrees[i] = node->subtrees[i];
                subtrees[i + 1] = vacation_right_subtree;
            }
            else
            {
                subtrees[i + (i < index_to_vacate
                    ? 0
                    : 1)] = node->subtrees[i];
            }
        }

        if (node->subtrees[0] == nullptr && split_mode == search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_split_mode::copy_median_kvp)
        {
            subtrees[node->min_keys_count] = new_node_after_split;
        }

        for (auto i = 0; i <= node_max_keys_count; i++)
        {
            if (i < node->min_keys_count)
            {
                node->keys_and_values[i] = std::move(kvps[i]);
                node->subtrees[i] = subtrees[i];
            }
            else if (i == node->min_keys_count)
            {
                if (split_mode == search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_split_mode::do_not_copy_median_kvp)
                {
                    node->subtrees[i] = subtrees[i];
                }
                else if (split_mode == search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_split_mode::copy_median_kvp)
                {
                    new (new_node_after_split->keys_and_values) typename search_tree<tkey, tvalue, tkey_comparer>::key_value_pair(kvps[i]);
                    node->subtrees[i] = subtrees[i];
                    new_node_after_split->subtrees[0] = nullptr;
                }
            }
            else if (i > node->min_keys_count)
            {
                auto target_index = i - node->min_keys_count - !static_cast<int>(split_mode);

                new (new_node_after_split->keys_and_values + target_index) typename search_tree<tkey, tvalue, tkey_comparer>::key_value_pair(std::move(kvps[i]));

                new_node_after_split->subtrees[target_index] = subtrees[i];
            }
        }

        new_node_after_split->subtrees[new_node_after_split->min_keys_count + static_cast<int>(split_mode)] = subtrees[node_max_keys_count + 1];

        typename associative_container<tkey, tvalue>::key_value_pair result;
        result.key = split_mode == search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_split_mode::copy_median_kvp
            ? std::move(kvps[node->min_keys_count].key)
            : kvps[node->min_keys_count].key;
        result.value = split_mode == search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_split_mode::copy_median_kvp
            ? std::move(kvps[node->min_keys_count].value)
            : kvps[node->min_keys_count].value;

        return std::make_optional<std::pair<typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *, typename associative_container<tkey, tvalue>::key_value_pair> >(std::make_pair<typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *, typename associative_container<tkey, tvalue>::key_value_pair>(std::move(new_node_after_split), std::move(result)));
    }
    else
    {
        new (node->keys_and_values + node->involved_keys) typename associative_container<tkey, tvalue>::key_value_pair;

        for (auto i = node->involved_keys; i > index_to_vacate; --i)
        {
            node->keys_and_values[i].key = node->keys_and_values[i - 1].key;
            node->keys_and_values[i].value = std::move(node->keys_and_values[i - 1].value);
            node->subtrees[i + 1] = node->subtrees[i];
        }

        node->keys_and_values[index_to_vacate].key = key;
        node->keys_and_values[index_to_vacate].value = std::move(value);
        node->involved_keys++;
        node->subtrees[index_to_vacate + 1] = vacation_right_subtree;

        return std::nullopt;
    }
}

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
size_t search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::get_node_shell_size() const noexcept
{
    return sizeof(search_tree<tkey, tvalue, tkey_comparer>::search_tree_node);
}

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
void search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_constructor_call(
    typename search_tree<tkey, tvalue, tkey_comparer>::search_tree_node *allocated_memory) const noexcept
{
    new (allocated_memory) search_tree<tkey, tvalue, tkey_comparer>::search_tree_node;
}

template<typename tkey, typename tvalue, typename tkey_comparer>
void search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::node_additional_data_injector(
    search_tree::search_tree_node *initialized_memory) const noexcept
{

}

// region logger_holder implementation

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
logger *search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::get_logger() const noexcept
{
    return _logger;
}

// endregion logger_holder implementation

// region allocator_holder implementation

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
allocator *search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context::get_allocator() const noexcept
{
    return _allocator;
}

// endregion allocator_holder implementation

// endregion search_tree<tkey, tvalue, tkey_comparer>::node_interaction_context implementation

// region exceptions overrides

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
char const *search_tree<tkey, tvalue, tkey_comparer>::insertion_exception::what() const noexcept
{
    return "key to insert by is already exists in tree";
}

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
char const *search_tree<tkey, tvalue, tkey_comparer>::reading_exception::what() const noexcept
{
    return "key to read by not exists inside tree";
}

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
char const *search_tree<tkey, tvalue, tkey_comparer>::removing_exception::what() const noexcept
{
    return "key to remove by not exists inside tree";
}

// endregion exception overrides

// region rule of 5 implementation

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
search_tree<tkey, tvalue, tkey_comparer>::search_tree(
    search_tree<tkey, tvalue, tkey_comparer> const &other)
{
    _allocator = other._allocator;
    _logger = other._logger;
    _context = other.create_node_interaction_context(_logger, _allocator);
    _root = _context->copy_node(other._root, true);
}

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
search_tree<tkey, tvalue, tkey_comparer> &search_tree<tkey, tvalue, tkey_comparer>::operator=(
    search_tree<tkey, tvalue, tkey_comparer> const &other)
{
    if (this == &other)
    {
        return *this;
    }

    _context->destroy_node(_root, true);
    delete _context;

    _allocator = other._allocator;
    _logger = other._logger;
    _context = other.create_node_interaction_context(_logger, _allocator);

    _root = _context->copy_node(other._root, true);

    return *this;
}

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
search_tree<tkey, tvalue, tkey_comparer>::search_tree(
    search_tree<tkey, tvalue, tkey_comparer> &&other) noexcept
{
    _context = other._context;
    other._context = nullptr;

    _logger = other._logger;
    other._logger = nullptr;

    _allocator = other._allocator;
    other._allocator = nullptr;

    _root = other._root;
    other._root = nullptr;
}

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
search_tree<tkey, tvalue, tkey_comparer> &search_tree<tkey, tvalue, tkey_comparer>::operator=(
    search_tree<tkey, tvalue, tkey_comparer> &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    _context->destroy_node(_root, true);
    delete _context;

    _context = other._context;
    other._context = nullptr;

    _logger = other._logger;
    other._logger = nullptr;

    _allocator = other._allocator;
    other._allocator = nullptr;

    _root = other._root;
    other._root = nullptr;

    return *this;
}

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
search_tree<tkey, tvalue, tkey_comparer>::~search_tree()
{
    _context->destroy_node(_root, true);
    delete _context;
}

// endregion rule of 5 implementation

// region logger_holder implementation

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
logger *search_tree<tkey, tvalue, tkey_comparer>::get_logger() const noexcept
{
    return _logger;
}

// endregion logger_holder implementation

// region allocator_holder implementation

template<
    typename tkey,
    typename tvalue,
    typename tkey_comparer>
allocator *search_tree<tkey, tvalue, tkey_comparer>::get_allocator() const noexcept
{
    return _allocator;
}

// endregion allocator_holder implementation

// endregion search_tree<tkey, tvalue, tkey_comparer> implementation

#endif //DATA_STRUCTURES_CPP_SEARCH_TREE_H