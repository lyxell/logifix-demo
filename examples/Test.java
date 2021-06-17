class Test {
    static <K, V> V getOrCreate(Map<K, V> map, K key, Supplier<V> valueCreator, Consumer<V> initializer) {
        V value = map.get(key);
        if (value == null) {
            value = valueCreator.get();
            map.put(key, value);
            if (initializer != null) {
                initializer.accept(value);
            }
        }
        return value;
    }
}
