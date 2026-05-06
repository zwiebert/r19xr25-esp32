import tippy, { type Props } from 'tippy.js';
import type { Action } from 'svelte/action';

// Use Partial<Props> because you don't need to provide every single Tippy option
export const tooltip: Action<HTMLElement, Partial<Props>> = (node, options) => {
  const instance = tippy(node, options ?? {});

  return {
    update(newOptions) {
      // This is the reactive part for Svelte 5
      instance.setProps(newOptions ?? {});
    },
    destroy() {
      instance.destroy();
    }
  };
}