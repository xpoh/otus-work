box.schema.create_space("friends")
box.space.friends:create_index('primary', {type="TREE", unique=true, parts={1, 'unsigned'}})
box.space.friends:create_index('user_id', {type="TREE", unique=false, parts={2, 'string'}})

function get_friends(id)
    return box.space.friends.index.user_id:select(id)
end
