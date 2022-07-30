function updateCollisionMeshes(Bodies,numModules)
%UPDATECOLLISIONMESHES update collision meshes of all bodies in rbtree

for lv = 1:numModules 
    updateCollisionMesh(Bodies{lv});
end

end

