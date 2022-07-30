classdef RigidMoriInternal < robotics.manip.internal.RigidBody
    properties (Dependent) %TODO sort out access restricitons
        ChildLocation
        EdgeLengths
        xyt1t2
        updateColMesh
    end
    
    properties (Access = ?robotics.manip.internal.InternalAccess)
        ChildLocationInternal
        EdgeLengthsInternal
        xyt1t2Internal
        updateColMeshInternal
    end

    methods
        function obj = RigidMoriInternal(name)
            % TODO would be awesome to modify the addBody function aswell
            % Call asset constructor
            obj@robotics.manip.internal.RigidBody(name);
            %Initialize user added variables
            obj.ChildLocationInternal = [];
            obj.EdgeLengthsInternal = [0.186 0.186 0.186];
            obj.xyt1t2Internal = {0.093 sin(pi/3)*0.186 pi/3 pi/3};
            obj.updateColMeshInternal = true;
        end
        
        function newbody = copy(obj)
        %COPY Creates a deep copy of the rigid body object. The copied
        %   body will not retain any parent/children information as it
        %   is a standalone body.

            newbody = RigidMoriInternal(obj.NameInternal);
            newbody.JointInternal = copy(obj.JointInternal);

            newbody.MassInternal = obj.MassInternal;
            newbody.CenterOfMassInternal = obj.CenterOfMassInternal;
            newbody.InertiaInternal = obj.InertiaInternal;
            newbody.SpatialInertia = obj.SpatialInertia;

            if coder.target('matlab')
                newbody.VisualsInternal = obj.VisualsInternal;
                newbody.ChildLocationInternal = obj.ChildLocationInternal; %NOTE !!!Added!!!
                newbody.EdgeLengthsInternal = obj.EdgeLengthsInternal; %NOTE !!!Added!!!
                newbody.xyt1t2Internal = obj.xyt1t2Internal; %NOTE !!!Added!!!
            end
            newbody.CollisionsInternal = copy(obj.CollisionsInternal);
        end
        
        function value = get.ChildLocation(obj)
            %get.ChildLocation
            value = obj.ChildLocationInternal;
        end
        
        function set.ChildLocation(obj, value)
        %set.ChildLocation
            %Should I remove the set function and handle it internally to
            %addBody?
            obj.ChildLocationInternal = value;
        end
        
        function value = get.EdgeLengths(obj)
        %get.EdgeLengths
            value = obj.EdgeLengthsInternal;
        end
        
        function set.EdgeLengths(obj, value)
        %set.EdgeLengths
            obj.EdgeLengthsInternal = value;
        end
        
        function value = get.xyt1t2(obj)
        %get.xyt1t2t
            value = obj.xyt1t2Internal;
        end
        
        function set.xyt1t2(obj, value)
        %set.xyt1t2
            obj.xyt1t2Internal = value;
        end
        
        function value = get.updateColMesh(obj)
        %get.updateColMesh
            value = obj.updateColMeshInternal;
        end
        
        function set.updateColMesh(obj, value)
        %set.updateColMesh
            obj.updateColMeshInternal = value;
        end
        
    end
end
