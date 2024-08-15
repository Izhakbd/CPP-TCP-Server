class Video {
    private:
        int id;
        int views;

    public:
        // Constructor
        Video(int id, int views) : id(id), views(views) {}

        // Getter for id
        int getId() const {
            return id;
        }

        // Getter for views
        int getViews() const {
            return views;
        }

        // Setter for id
        void setId(int id) {
            id = id;
        }

        // Setter for views
        void setViews(int views) {
            views = views;
        }
};
