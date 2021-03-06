#undef I3__FILE__
#define I3__FILE__ "output.c"
/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * output.c: Output (monitor) related functions.
 *
 */
#include "all.h"

/*
 * Returns the output container below the given output container.
 *
 */
Con *output_get_content(Con *output) {
    Con *child;

    TAILQ_FOREACH(child, &(output->nodes_head), nodes)
    if (child->type == CT_CON)
        return child;

    return NULL;
}

/*
 * Returns an 'output' corresponding to one of left/right/down/up or a specific
 * output name.
 *
 */
Output *get_output_from_string(Output *current_output, const char *output_str) {
    Output *output;

    if (strcasecmp(output_str, "left") == 0)
        output = get_output_next_wrap(D_LEFT, current_output);
    else if (strcasecmp(output_str, "right") == 0)
        output = get_output_next_wrap(D_RIGHT, current_output);
    else if (strcasecmp(output_str, "up") == 0)
        output = get_output_next_wrap(D_UP, current_output);
    else if (strcasecmp(output_str, "down") == 0)
        output = get_output_next_wrap(D_DOWN, current_output);
    else
        output = get_output_by_name(output_str);

    return output;
}

/*
 * Iterates over all outputs and pushes sticky windows to the currently visible
 * workspace on that output.
 *
 */
void output_push_sticky_windows(void) {
    Con *output;
    TAILQ_FOREACH(output, &(croot->nodes_head), nodes) {
        Con *workspace, *visible_ws = NULL;
        GREP_FIRST(visible_ws, output_get_content(output), workspace_is_visible(child));

        /* We use this loop instead of TAILQ_FOREACH to avoid problems if the
         * sticky window was the last window on that workspace as moving it in
         * this case will close the workspace. */
        for (workspace = TAILQ_FIRST(&(output_get_content(output)->nodes_head));
             workspace != TAILQ_END(&(output_get_content(output)->nodes_head));) {
            Con *current_ws = workspace;
            workspace = TAILQ_NEXT(workspace, nodes);

            /* Since moving the windows actually removes them from the list of
             * floating windows on this workspace, here too we need to use
             * another loop than TAILQ_FOREACH. */
            Con *child;
            for (child = TAILQ_FIRST(&(current_ws->floating_head));
                 child != TAILQ_END(&(current_ws->floating_head));) {
                Con *current = child;
                child = TAILQ_NEXT(child, floating_windows);

                if (con_is_sticky(current))
                    con_move_to_workspace(current, visible_ws, true, false, true);
            }
        }
    }
}
