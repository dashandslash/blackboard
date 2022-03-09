#include "sequencer.h"

#include <iostream>

namespace blackboard::gui {

Sequencer::Sequencer()
{
    // sequence with default values

    sequence.m_frame_min = -100;
    sequence.m_frame_max = 1000;
    sequence.m_items.push_back(Sequence::SequenceItem{0, 10, 30, false});
    sequence.m_items.push_back(Sequence::SequenceItem{1, 20, 30, true});
    sequence.m_items.push_back(Sequence::SequenceItem{3, 12, 60, false});
    sequence.m_items.push_back(Sequence::SequenceItem{2, 61, 90, false});
    sequence.m_items.push_back(Sequence::SequenceItem{4, 90, 99, false});
}

void Sequencer::draw()
{
    ImGui::Begin("Sequencer");
    // let's create the sequencer
    static int selectedEntry = -1;
    static int firstFrame = 0;
    static bool expanded = true;
    static int currentFrame = 100;
    static bool play{false};
    static bool follow_cursor{false};

    ImGui::Checkbox("Play", &play);
    if (play)
    {
        currentFrame = currentFrame < sequence.m_frame_max ? ++currentFrame : 0;
    }
    ImGui::SameLine();
    ImGui::Checkbox("Follow Cursor", &follow_cursor);
    if (follow_cursor)
    {
        firstFrame = currentFrame - 10.0;
    }
    ImGui::SameLine();
    ImGui::PushItemWidth(130);
    ImGui::InputInt("Frame Min", &sequence.m_frame_min);
    ImGui::SameLine();
    ImGui::InputInt("Frame ", &currentFrame);
    ImGui::SameLine();
    ImGui::InputInt("Frame Max", &sequence.m_frame_max);
    ImGui::PopItemWidth();
    ImSequencer::Sequencer(&sequence, &currentFrame, &expanded, &selectedEntry, &firstFrame,
                           ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_CHANGE_FRAME |
                             ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL |
                             ImSequencer::SEQUENCER_COPYPASTE);
    // add a UI to edit that particular item
    if (selectedEntry != -1)
    {
        //        const the_sequence::the_sequenceItem &item = the_sequence.m_items[selectedEntry];
        //        ImGui::Text("I am a %s, please edit me", SequencerItemTypeNames[item.mType]);
        // switch (type) ....
    }
    ImGui::End();
}
}    // namespace blackboard::gui
