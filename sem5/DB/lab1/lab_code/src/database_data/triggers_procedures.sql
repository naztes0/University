-- FUNCTION: public.set_timestamp()
-- PURPOSE: Automatically sets the 'updated_at' column to the current time.
CREATE OR REPLACE FUNCTION public.set_timestamp()
RETURNS trigger
LANGUAGE plpgsql
AS $$
BEGIN
  NEW.updated_at = NOW();
  RETURN NEW;
END;
$$;


-- PROCEDURE: soft_delete_property
-- PURPOSE: Implements the 'Soft Delete' logic for the property table (is_deleted = TRUE).
CREATE OR REPLACE PROCEDURE soft_delete_property(
    p_property_id INTEGER,
    p_updated_by INTEGER
)
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE public.property
    SET
        is_deleted = TRUE,
        updated_by = p_updated_by
    WHERE
        property_id = p_property_id;
END;
$$;

-- PROCEDURE: restore_property
-- PURPOSE: Reverts the soft deletion (is_deleted = FALSE).
CREATE OR REPLACE PROCEDURE restore_property(
    p_property_id INTEGER,
    p_updated_by INTEGER
)
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE public.property
    SET
        is_deleted = FALSE,
        updated_by = p_updated_by
    WHERE
        property_id = p_property_id;
END;
$$;

-- FUNCTION: get_active_property_count
-- PURPOSE: Returns the total count of properties that are NOT soft-deleted.
CREATE OR REPLACE FUNCTION get_active_property_count()
RETURNS BIGINT
LANGUAGE sql
AS $$
    SELECT COUNT(property_id)
    FROM public.property
    WHERE is_deleted = FALSE;
$$;


-- TRIGGER: set_users_timestamp (users)
CREATE OR REPLACE TRIGGER set_users_timestamp
BEFORE UPDATE ON public.users
FOR EACH ROW
EXECUTE FUNCTION public.set_timestamp();

-- TRIGGER: set_owner_timestamp (owner)
CREATE OR REPLACE TRIGGER set_owner_timestamp
BEFORE UPDATE ON public.owner
FOR EACH ROW
EXECUTE FUNCTION public.set_timestamp();

-- TRIGGER: set_buyer_timestamp (buyer)
CREATE OR REPLACE TRIGGER set_buyer_timestamp
BEFORE UPDATE ON public.buyer
FOR EACH ROW
EXECUTE FUNCTION public.set_timestamp();

-- TRIGGER: set_tenant_timestamp (tenant)
CREATE OR REPLACE TRIGGER set_tenant_timestamp
BEFORE UPDATE ON public.tenant
FOR EACH ROW
EXECUTE FUNCTION public.set_timestamp();

-- TRIGGER: set_property_timestamp (property)
CREATE OR REPLACE TRIGGER set_property_timestamp
BEFORE UPDATE ON public.property
FOR EACH ROW
EXECUTE FUNCTION public.set_timestamp();

-- TRIGGER: set_rental_timestamp (rental_agreement)
CREATE OR REPLACE TRIGGER set_rental_timestamp
BEFORE UPDATE ON public.rental_agreement
FOR EACH ROW
EXECUTE FUNCTION public.set_timestamp();

-- TRIGGER: set_sale_timestamp (sale_agreement)
CREATE OR REPLACE TRIGGER set_sale_timestamp
BEFORE UPDATE ON public.sale_agreement
FOR EACH ROW
EXECUTE FUNCTION public.set_timestamp();

-- TRIGGER FUNCTION: soft_delete_property_on_sale
-- PURPOSE: Executes soft delete on 'property' when 'sale_agreement' status changes to 'Closed'.
CREATE OR REPLACE FUNCTION soft_delete_property_on_sale()
RETURNS TRIGGER
LANGUAGE plpgsql
SECURITY DEFINER
AS $$
DECLARE
    sold_status_id INTEGER;
BEGIN
    SELECT status_id INTO sold_status_id
    FROM public.contract_status
    WHERE status_name = 'Closed'
    LIMIT 1;
    IF NEW.status_id = sold_status_id AND NEW.status_id IS DISTINCT FROM OLD.status_id THEN
        UPDATE public.property
        SET
            is_deleted = TRUE,
            updated_by = NEW.updated_by
        WHERE
            property_id = NEW.property_id;
    END IF;

    RETURN NEW;
END;
$$;

-- TRIGGER: trigger_soft_delete_property (sale_agreement)
CREATE OR REPLACE TRIGGER trigger_soft_delete_property
AFTER UPDATE ON public.sale_agreement
FOR EACH ROW
EXECUTE FUNCTION soft_delete_property_on_sale();


-- TRIGGER FUNCTION: set_work_assignment_completion_date
-- PURPOSE: Automatically sets 'completed_at' when 'work_assignment' status changes to 'Completed'.
CREATE OR REPLACE FUNCTION set_work_assignment_completion_date()
RETURNS TRIGGER
LANGUAGE plpgsql
AS $$
DECLARE
    completed_status_id INTEGER;
BEGIN
    SELECT status_id INTO completed_status_id
    FROM public.maintenance_status
    WHERE status_name = 'Completed'
    LIMIT 1;
    IF NEW.status_id = completed_status_id AND (OLD.status_id IS DISTINCT FROM NEW.status_id OR NEW.completed_at IS NULL) THEN
        NEW.completed_at = NOW();
    ELSIF OLD.status_id = completed_status_id AND NEW.status_id IS DISTINCT FROM completed_status_id THEN
        NEW.completed_at = NULL;
    END IF;
    RETURN NEW;
END;
$$;

-- TRIGGER: trigger_set_assignment_completion (work_assignment)
CREATE OR REPLACE TRIGGER trigger_set_assignment_completion
BEFORE UPDATE ON public.work_assignment
FOR EACH ROW
EXECUTE FUNCTION set_work_assignment_completion_date();


-- TRIGGER FUNCTION: prevent_agreement_deletion_with_payments
-- PURPOSE: Prevents hard deletion of a rental agreement if related payments exist (Data Integrity).
CREATE OR REPLACE FUNCTION prevent_agreement_deletion_with_payments()
RETURNS TRIGGER
LANGUAGE plpgsql
AS $$
DECLARE
    payment_count INTEGER;
BEGIN
    SELECT COUNT(*) INTO payment_count
    FROM public.payment
    WHERE rental_id = OLD.rental_id;

    IF payment_count > 0 THEN
        RAISE EXCEPTION 'Cannot delete rental agreement ID %, because related payments (%s) exist.', OLD.rental_id, payment_count;
    END IF;

    RETURN OLD;
END;
$$;

-- TRIGGER: trigger_prevent_rental_deletion (rental_agreement)
CREATE OR REPLACE TRIGGER trigger_prevent_rental_deletion
BEFORE DELETE ON public.rental_agreement
FOR EACH ROW
EXECUTE FUNCTION prevent_agreement_deletion_with_payments();


-- TRIGGER FUNCTION: create_initial_work_assignment
-- PURPOSE: Automatically creates a maintenance task ('New' status) after a new rental agreement is signed.
CREATE OR REPLACE FUNCTION create_initial_work_assignment()
RETURNS TRIGGER
LANGUAGE plpgsql
AS $$
BEGIN
    INSERT INTO public.work_assignment (
        user_id,
        property_id,
        description,
        status_id,
        created_at
    )
    VALUES (
        NEW.created_by,
        NEW.property_id,
        'Perform inspection before rental start: ' || NEW.rental_id,
        (SELECT status_id FROM public.maintenance_status WHERE status_name = 'New' LIMIT 1),
        NOW()
    );
    RETURN NEW;
END;
$$;

-- TRIGGER: trigger_new_rental_assignment (rental_agreement)
CREATE OR REPLACE TRIGGER trigger_new_rental_assignment
AFTER INSERT ON public.rental_agreement
FOR EACH ROW
EXECUTE FUNCTION create_initial_work_assignment();


--VIEWS for Repository pattern

-- VIEW: active_properties_view
-- PURPOSE: Provides a filtered and joined list of active properties (is_deleted = FALSE)
CREATE OR REPLACE VIEW active_properties_view AS
SELECT
    p.property_id,
    p.address,
    pt.property_name AS property_type,
    p.property_area,
    p.rent_price,
    p.sale_price,
    o.first_name AS owner_first_name,
    o.last_name AS owner_last_name,
    p.created_at,
    p.updated_at,
    u.first_name AS updated_by_user
FROM
    public.property p
JOIN
    public.property_type pt ON p.property_type = pt.property_type_id
JOIN
    public.owner o ON p.owner_id = o.owner_id
LEFT JOIN
    public.users u ON p.updated_by = u.user_id
WHERE
    p.is_deleted = FALSE;


-- VIEW: sale_agreement_details_view
-- PURPOSE: Provides comprehensive details of all sale agreements, including buyer and property information.
CREATE OR REPLACE VIEW sale_agreement_details_view AS
SELECT
    sa.sale_id,
    sa.sale_price,
    sa.sale_date,
    cs.status_name AS contract_status,
    p.address AS property_address,
    b.first_name AS buyer_first_name,
    b.last_name AS buyer_last_name,
    b.email AS buyer_email,
    sa.created_at
FROM
    public.sale_agreement sa
JOIN
    public.contract_status cs ON sa.status_id = cs.status_id
JOIN
    public.property p ON sa.property_id = p.property_id
JOIN
    public.buyer b ON sa.buyer_id = b.buyer_id;

-- VIEW: maintenance_tasks_view
-- PURPOSE: Provides an overview of all work assignments (tasks), linking them to the property, the requester, and the worker.
CREATE OR REPLACE VIEW maintenance_tasks_view AS
SELECT
    wa.assignment_id,
    wa.description,
    ms.status_name AS current_status,
    p.address AS property_address,
    u_requester.first_name AS requested_by_user,
    mw_user.first_name AS assigned_worker,
    wa.created_at,
    wa.completed_at
FROM
    public.work_assignment wa
JOIN
    public.property p ON wa.property_id = p.property_id
JOIN
    public.users u_requester ON wa.user_id = u_requester.user_id
JOIN
    public.maintenance_status ms ON wa.status_id = ms.status_id
LEFT JOIN
    public.maintenance_worker mw ON wa.worker_id = mw.worker_id
LEFT JOIN
    public.users mw_user ON mw.user_id = mw_user.user_id;


-- INDEX: idx_owner_last_name (B-tree)
CREATE INDEX idx_owner_last_name ON public.owner USING btree (last_name);

-- INDEX: idx_property_address_gin (GIN)
CREATE INDEX idx_property_address_gin ON public.property USING gin (to_tsvector('english', address));