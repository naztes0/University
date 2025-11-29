package com.util;

import com.model.Knife;
import java.util.Comparator;

/**
 * Comparators for sorting Knife objects
 */
public class KnifeComparator {

        public static final Comparator<Knife> BY_TYPE = Comparator.comparing(knife -> knife.getType().name());

        public static final Comparator<Knife> BY_ORIGIN = Comparator.comparing(Knife::getOrigin);

        public static final Comparator<Knife> BY_HANDY = Comparator.comparing(knife -> knife.getHandy().name());

        public static final Comparator<Knife> BY_BLADE_LENGTH = Comparator.comparingInt(
                        knife -> knife.getVisual() == null ? 0 : knife.getVisual().getBlade().getLength());

        public static final Comparator<Knife> BY_COLLECTIBLE = Comparator
                        .comparing(knife -> knife.getValue().isCollectible());

        public static final Comparator<Knife> BY_TYPE_AND_ORIGIN = BY_TYPE.thenComparing(BY_ORIGIN);

        public static final Comparator<Knife> BY_VALUE_AND_LENGTH = BY_COLLECTIBLE.thenComparing(BY_BLADE_LENGTH);
}