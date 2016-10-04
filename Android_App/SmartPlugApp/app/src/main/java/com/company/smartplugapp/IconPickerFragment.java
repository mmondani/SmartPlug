package com.company.smartplugapp;


import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;


/**
 * A simple {@link Fragment} subclass.
 */
public class IconPickerFragment extends Fragment {

    private GridView mIconGrid;

    private OnIconItemClickedInterface mCLickListener;



    public IconPickerFragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View v = inflater.inflate(R.layout.fragment_icon_picker, container, false);

        mIconGrid = (GridView) v.findViewById(R.id.frag_icon_picker_grid_view);
        mIconGrid.setAdapter(new GridIconAdapter(getActivity()));
        mIconGrid.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                IconItem item = IconItem.getItem((int) id);

                if (mCLickListener != null)
                    mCLickListener.onIconItemClicked(item.getResId());
            }
        });

        return v;
    }


    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);

        try {
            mCLickListener = (OnIconItemClickedInterface)activity;
        }
        catch (ClassCastException cce) {
            throw new ClassCastException(activity.toString() + " debe implementar OnItemClickListener");
        }
    }

    @Override
    public void onDetach() {
        super.onDetach();

        mCLickListener = null;
    }


    public interface OnIconItemClickedInterface {
        void onIconItemClicked (int resId);
    }

    /**
     * *********************************************************************************************
     * Adaptador del GridView para el menú con íconos
     * *********************************************************************************************
     */
    public class GridIconAdapter extends BaseAdapter {
        private Context context;

        public GridIconAdapter (Context context) {
            this.context = context;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {

            if (convertView == null) {
                LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                convertView = inflater.inflate(R.layout.icon_grid_item, parent, false);
            }

            ImageView image = (ImageView) convertView.findViewById(R.id.icon_grid_image);

            final IconItem item = getItem(position);
            image.setImageResource(item.getResId());

            return convertView;
        }

        @Override
        public int getCount() {
            return IconItem.getCount();
        }

        @Override
        public IconItem getItem(int position) {
            return IconItem.IconItems[position];
        }

        @Override
        public long getItemId(int position) {
            return IconItem.IconItems[position].getId();
        }
    }
}
