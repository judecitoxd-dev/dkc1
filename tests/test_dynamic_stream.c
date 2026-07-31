#include <assert.h>
#include "dk1/dynamic_stream.h"
int main(void) {
    Dk1DynamicStreamState s={0}; Dk1DynamicStreamUpdate u; Dk1SceneView v={0,0,384,224};
    assert(dk1_dynamic_stream_update(v,128u,&s,&u));
    assert(u.changed && u.new_first==0u && u.new_last==11u && u.columns_entered==12u);
    v.camera_x=16u; assert(dk1_dynamic_stream_update(v,128u,&s,&u));
    assert(u.changed && u.new_last==12u && u.columns_entered==1u);
    v.camera_x=32u; assert(dk1_dynamic_stream_update(v,128u,&s,&u));
    assert(u.changed && u.new_first==1u && u.columns_left==1u);
    assert(s.generation==3u);
    v.camera_x=32u; assert(dk1_dynamic_stream_update(v,128u,&s,&u)); assert(!u.changed);
    return 0;
}
